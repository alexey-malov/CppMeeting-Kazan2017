#include "BindWeakPtr.h"
#include <string>
#include <iostream>
#include <map>
#include <boost/utility/string_view.hpp>

using namespace std;
using boost::string_view;



struct ImageData {};

struct IImageLoader
{
	using Callback = function<void(string_view url, unique_ptr<ImageData> optData)>;

	virtual ~IImageLoader() = default;

	virtual void LoadImageAsync(string_view url, Callback callback) = 0;
};

struct ImageView : enable_shared_from_this<ImageView>
{
	ImageView(IImageLoader& loader) :m_loader(loader) {}

	void SetImageUrl(string_view url)
	{
		m_url = url.to_string();
#if 0
		weak_ptr<ImageView> weakSelf = shared_from_this();

		m_loader.LoadImageAsync(url, [weakSelf](auto url, auto data) {
			if (auto strongSelf = weakSelf.lock())
				strongSelf->OnImageLoaded(url, move(data));
		});
#else
		using namespace /*ph =*/ std::placeholders;
		m_loader.LoadImageAsync(url, BindWeakPtr(&ImageView::OnImageLoaded, shared_from_this(), _1, _2));
#endif
	}

private:
	void OnImageLoaded(string_view url, unique_ptr<ImageData> data)
	{
		if (m_url == url)
		{
			m_imageData = move(data);
			RedrawImage();
		}
	}

	void RedrawImage()
	{
		cout << "Redrawing image " << m_imageData.get() << " from URL " << m_url << "\n";
	}

	IImageLoader& m_loader;
	std::string m_url;
	unique_ptr<ImageData> m_imageData;
};

struct MockLoader final : IImageLoader
{
	void LoadImageAsync(string_view url, Callback callback) override
	{
		m_requests.emplace(url, callback);
	}

	void SetImageData(string_view url, unique_ptr<ImageData> data)
	{
		auto it = m_requests.find(url.to_string());
		if (it != m_requests.end())
		{
			it->second(url, move(data));
			m_requests.erase(it);
		}
	}
	map<string, Callback> m_requests;
};

int main()
{
	MockLoader ldr;

	auto imgView1 = make_shared<ImageView>(ldr);
	auto imgView2 = make_shared<ImageView>(ldr);
	imgView1->SetImageUrl("image1.png");
	imgView2->SetImageUrl("image2.png");
	ldr.SetImageData("image2.png", make_unique<ImageData>());
	imgView1.reset();
	ldr.SetImageData("image1.png", make_unique<ImageData>());
}