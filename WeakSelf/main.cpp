#include "BindWeakPtr.h"
#include <string>
#include <iostream>
#include <map>
#include <boost/utility/string_view.hpp>

using namespace std;
using boost::string_view;
using namespace std::placeholders;

struct Image { /* some image data */ };
using Callback = function<void(string_view url, unique_ptr<Image> image)>;
// Loads an image asynchronously
void LoadImageAsync(string_view url, Callback callback);

namespace
{
map<string, Callback> g_requests;
void FinishImageLoading(string_view url, unique_ptr<Image> img)
{
	auto it = g_requests.find(url.to_string());
	if (it != g_requests.end())
	{
		it->second(url, move(img));
		g_requests.erase(it);
	}
}
}

struct ImageView
{
	void ShowImageAtURL(string_view url)
	{
		/* Загрузить изображение асинхронно и вызвать OnImageLoaded */
	}
private:
	void OnImageLoaded(string_view url, unique_ptr<Image> image)
	{
		/* Сохранить изображение, обновить контрол */
	}
	unique_ptr<Image> m_image;
};

struct BadImageView
{
	void ShowImageAtURL(string_view url)
	{
		LoadImageAsync(url, [this](auto url, auto img) {
			OnImageLoaded(url, move(img)); 
		});
	}
private:
	void OnImageLoaded(string_view url, unique_ptr<Image> image)
	{
		m_image = move(image);
		cout << "Image loaded. URL: " << url << " Addr: " << m_image.get() << "\n";
	}
	unique_ptr<Image> m_image;
};

// Image View implementation using explicit weak self handling
struct ImageView1 : enable_shared_from_this<ImageView1>
{
	static auto Create()
	{
		return shared_ptr<ImageView1>(new ImageView1);
	}
	void ShowImageAtURL(string_view url)
	{
		weak_ptr<ImageView1> weakSelf = shared_from_this();
		LoadImageAsync(url, [weakSelf](auto url, auto data) {
			if (auto strongSelf = weakSelf.lock())
				strongSelf->OnImageLoaded(url, move(data));
		});
	}
private:
	ImageView1() = default;
	void OnImageLoaded(string_view url, unique_ptr<Image> image)
	{
		m_image = move(image);
		cout << "Image loaded. URL: " << url << " Addr: " << m_image.get() << "\n";
	}
	unique_ptr<Image> m_image;
};

// Image View implementation using explicit BindWeakPtr helper
struct ImageView2 : enable_shared_from_this<ImageView2>
{
	static auto Create()
	{
		return shared_ptr<ImageView2>(new ImageView2);
	}
	void ShowImageAtURL(string_view url)
	{
		LoadImageAsync(url, BindWeakPtr(&ImageView2::OnImageLoaded, shared_from_this() /* or weak_from_this() since C++17 */, _1, _2));
	}
private:
	ImageView2() = default;
	void OnImageLoaded(string_view url, unique_ptr<Image> image)
	{
		m_image = move(image);
		cout << "Image loaded. URL: " << url << " Addr: " << m_image.get() << "\n";
	}
	unique_ptr<Image> m_image;
};

struct ImageView3
{
	ImageView3() :m_impl(make_shared<Impl>()) {}
	void ShowImageAtURL(string_view url)
	{
		m_impl->ShowImageAtURL(url);
	}
private:
	struct Impl : enable_shared_from_this<Impl> {
		void ShowImageAtURL(string_view url) {
			LoadImageAsync(url, BindWeakPtr(&Impl::OnImageLoaded, shared_from_this() /* or weak_from_this() since C++17 */, _1, _2));
		}
		void OnImageLoaded(string_view url, unique_ptr<Image> image)
		{
			m_image = move(image);
			cout << "Image loaded. URL: " << url << " Addr: " << m_image.get() << "\n";
		}
		unique_ptr<Image> m_image;
	};

	shared_ptr<Impl> m_impl;
};

int main()
{
#if 0 // This code will cause UB
	{
		auto imgView = make_shared<BadImageView>();
		imgView->ShowImageAtURL("image1.png");
		FinishImageLoading("image1.png", make_unique<Image>());
		
		imgView->ShowImageAtURL("image2.png");
		imgView.reset();
		FinishImageLoading("image2.png", make_unique<Image>()); // will cause UB by calling method on deleted object
	}
#endif
	{
		auto imgView = ImageView1::Create();
		imgView->ShowImageAtURL("image1.png");
		FinishImageLoading("image1.png", make_unique<Image>());

		imgView->ShowImageAtURL("image2.png");
		imgView.reset();
		FinishImageLoading("image2.png", make_unique<Image>()); // will ignore loaded image
	}
	{
		auto imgView = ImageView2::Create();
		imgView->ShowImageAtURL("image1.png");
		FinishImageLoading("image1.png", make_unique<Image>());

		imgView->ShowImageAtURL("image2.png");
		imgView.reset();
		FinishImageLoading("image2.png", make_unique<Image>()); // will ignore loaded image
	}
	{
		{
			ImageView3 imgView;
			imgView.ShowImageAtURL("image1.png");
			FinishImageLoading("image1.png", make_unique<Image>());

			imgView.ShowImageAtURL("image2.png");

		}
		FinishImageLoading("image2.png", make_unique<Image>()); // will ignore loaded image
	}
}

void LoadImageAsync(string_view url, Callback callback)
{
	g_requests.emplace(url, callback);
}

