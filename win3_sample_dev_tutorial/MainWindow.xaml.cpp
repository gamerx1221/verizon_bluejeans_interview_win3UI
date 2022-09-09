
/*********************************************************
* Author: Navjot S. Sandhu
* Description: Interview Project for Verizon BlueJeans; EPAM Staffing
*              Allows playback of images in a directory at 15 fps, and animated gifs, with each frame being split between 4 sub-images
*              in corners with space horizontally and vertically between
*Date: 9/7/2022
*Technology: WinRT, Window Sdk App using c++
*MainWindow:  Primary Application class with user interface, that loads and displays images
**************************************************/
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Microsoft.Windows.System.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.h>

#include <vector>
#include <xstring>
#include <algorithm>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Media::Imaging;
//from documentation and examples
struct __declspec(uuid("905a0fef-bc53-11df-8c49-001e4fc686da")) IBufferByteAccess : ::IUnknown
{
    virtual HRESULT __stdcall Buffer(uint8_t** value) = 0;
};

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::win3_sample_dev_tutorial::implementation
{ 
        //creates rect dimenension sets for corner images
        std::vector<winrt::Windows::Foundation::Rect> getRects(float width, float height) {
        std::vector<winrt::Windows::Foundation::Rect> rects;
        rects.push_back({ 0, 0, width, height });
        rects.push_back({ width,0, width, height });
        rects.push_back({ 0, height, width, height });
        rects.push_back({ width,height, width, height });
        return rects;
    }
   
    void MainWindow::setDimensionalValues() {

        int space_width = 200; //width between corner images
        int space_height = 50; //height between corner images


        int width = this->Bounds().Width - space_width;
        int height = this->Bounds().Height - space_height;
        imageWidth = width;
        imageHeight = height;

        float img_width = width / 2.0f;
        float img_height = height / 2.0f;
        this->primaryCanvas().SetLeft(Start_Stop(), (this->Bounds().Width / 2) - Start_Stop().Width());
        this->primaryCanvas().SetTop(Start_Stop(), (this->Bounds().Height / 2)- Start_Stop().Height());
       
        this->primaryCanvas().SetLeft(filesLoading(), this->Bounds().Width / 2);
        this->primaryCanvas().SetTop(filesLoading(), this->Bounds().Height / 2);

        this->primaryCanvas().SetLeft(TopLeftImg1(), 0.0);
        this->primaryCanvas().SetTop(TopLeftImg1(), 0.0);

        this->primaryCanvas().SetLeft(TopRightImg1(), this->Bounds().Width - width);
        this->primaryCanvas().SetTop(TopRightImg1(), 0.0);

        this->primaryCanvas().SetLeft(BottomLeftImg1(), 0.0);
        this->primaryCanvas().SetTop(BottomLeftImg1(), this->Bounds().Height - height);

        this->primaryCanvas().SetLeft(BottomRightImg1(), this->Bounds().Width - width);
        this->primaryCanvas().SetTop(BottomRightImg1(), this->Bounds().Height - height);

        this->TopLeftImg1().Width(width);
        this->TopLeftImg1().Height(height);
        
        this->TopRightImg1().Width(width);
        this->TopRightImg1().Height(height);

        this->BottomLeftImg1().Width(width);
        this->BottomLeftImg1().Height(height);

        this->BottomRightImg1().Width(width);
        this->BottomRightImg1().Height(height);
   
   
        this->topLeftRect1().Rect({0, 0, img_width, img_height});
        this->topRightRect1().Rect({ img_width,0, img_width, img_height });
        this->bottomLeftRect1().Rect({ 0, img_height, img_width, img_height });
        this->bottomRightRect1().Rect({ img_width,img_height, img_width, img_height });

    }

    void MainWindow::setImageSource() {
     
        if (totalNumImages == 0 && numAnimGifs == 0) {
            numAnimGifs = animated_gifs.Size();
            currentGifIndex = 0;
        }
        if (numAnimGifs > 0) {
            auto img = animated_gifs.begin() + currentGifIndex++;
            TopLeftImg1().Source((*img));
            TopRightImg1().Source(*img);
            BottomLeftImg1().Source(*img);
            BottomRightImg1().Source(*img);
            (*img).Play();
            numAnimGifs--;
            
            gifs_playing = true;
            
            animGifCount = 0;
      
            timer.Start();
        }
        else {
            if (totalNumImages > 0) {
                auto img = images.begin() + currentImgIndex;
                TopLeftImg1().Source((*img));
                TopRightImg1().Source(*img);
                BottomLeftImg1().Source(*img);
                BottomRightImg1().Source(*img);

                currentImgIndex++;
                if (currentImgIndex == totalNumImages) {
                    currentImgIndex = 0;
                    currentGifIndex = 0;
                    numAnimGifs = animated_gifs.Size();
                    // toggleTimer();
                     timer.Start();
                }
                else {
                    timer.Start();
                }
            }
        }
    }
    //local function that checks filename of file to see if it is an image; will return flag if it is a GIF file
    bool checkFileType(std::wstring filename, bool* isGif) {
        std::wstring filename_ = filename;
        std::transform(filename_.begin(), filename_.end(), filename_.begin(), ::tolower);
        if (filename_.find(L".jpg") != std::wstring::npos)
            return true;
        if (filename_.find(L".png") != std::wstring::npos)
            return true;
        if (filename_.find(L".bmp") != std::wstring::npos)
            return true;
        if (filename_.find(L".gif") != std::wstring::npos) {
            *isGif = true;
            return true;
        }
        return false;
    }

    IAsyncAction MainWindow::LoadFilesAsync(hstring folder_name)
    {
        // Get the app's installation folder.
        currentImgIndex = 0;
        std::wstring path{ Windows::ApplicationModel::Package::Current().InstalledLocation().Path() + L"\\Assets\\"+folder_name };

        // Get the folder object that corresponds to this absolute path in the file system.
        Windows::Storage::StorageFolder folder{ co_await Windows::Storage::StorageFolder::GetFolderFromPathAsync(path) };

        Windows::Storage::Search::StorageFileQueryResult results{ folder.CreateFileQuery()};

        filesInFolder=co_await results.GetFilesAsync();
        int count = 0;
        bool isGif = false;
        // Iterate over the results, and print the list of files to the Visual Studio output window.
        for (StorageFile const& fileInFolder : filesInFolder)
        {
            if (checkFileType(fileInFolder.Name().c_str(), &isGif)) {
                OutputDebugString(fileInFolder.Path().c_str());
                OutputDebugString(L"\n");
                IRandomAccessStream stream{ co_await fileInFolder.OpenAsync(FileAccessMode::Read) };
                //streams.Append(stream);

                auto decoder{ co_await Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream) };

                int frame_count = decoder.FrameCount();
                if (isGif && decoder.FrameCount() > 1) {
                    BitmapImage animatedGif{};
                    animatedGif.SetSourceAsync(stream);
                   // animatedGif.AutoPlay(true);
                    animated_gifs.Append(animatedGif);
                }
                else {
                    Windows::Graphics::Imaging::BitmapTransform transform;

                    Windows::Graphics::Imaging::PixelDataProvider pixelData{ co_await decoder.GetPixelDataAsync(
                        Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8, // WriteableBitmap uses BGRA format 
                        Windows::Graphics::Imaging::BitmapAlphaMode::Straight,
                        transform,
                        Windows::Graphics::Imaging::ExifOrientationMode::IgnoreExifOrientation, // This sample ignores Exif orientation 
                        Windows::Graphics::Imaging::ColorManagementMode::DoNotColorManage
                    ) };

                    // An array containing the decoded image data, which could be modified before being displayed 
                    winrt::com_array<uint8_t> sourcePixels{ pixelData.DetachPixelData() };

                    //note: originally just used a BitmapImage with streams

                    IWriteableBitmap bitmap_{};

                    images.Append(WriteableBitmap(decoder.PixelWidth(), decoder.PixelHeight()));
                    auto bufferByteAccess{ images.GetAt(images.Size() - 1).PixelBuffer().as<::IBufferByteAccess>() };
                    uint8_t* pTargetBytes{ nullptr };
                    bufferByteAccess->Buffer(&pTargetBytes);

                    for (auto& element : sourcePixels)
                    {
                        *(pTargetBytes++) = element;
                    }
                    count++;
                }
                stream.Close();

                std::wstring output{ fileInFolder.Name() };
                ::OutputDebugString(output.append(L"\n").c_str()); //outputs filename to console output for debugging purposes
            }
        }
        totalNumImages = count;
      
        filesLoading().ShowPaused(true);
        filesLoading().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

        timer.Start();
    }

    MainWindow::MainWindow(): images(winrt::multi_threaded_observable_vector<WriteableBitmap>())
        //, streams(winrt::multi_threaded_observable_vector<IRandomAccessStream>())
        , animated_gifs(winrt::multi_threaded_observable_vector<BitmapImage>())
    {
        InitializeComponent();
        auto getFiles{LoadFilesAsync(imgDir)};

        timer.Interval(std::chrono::milliseconds{ 66 });
        auto registrationtoken = timer.Tick({ this, &MainWindow::OnTick });
    }

    MainWindow::~MainWindow()
    {
        timer.Stop();
        images.Clear();
        animated_gifs.Clear();

        OutputDebugStringW(L"window closed\n");
    }

    void MainWindow::OnTick(Windows::Foundation::IInspectable const& /* sender */, Windows::Foundation::IInspectable const& /* e */)      
    {
        //change the source every 66.67 milliseconds for 15fps  //once it gets to end of image files start loop over
        //std::cout << "timer working" << endl;
        if (animated_gifs.Size() > 0 && currentGifIndex > 0) {
            if (animated_gifs.GetAt(currentGifIndex - 1).IsPlaying()) {
                animGifCount++;

                if (animGifCount == maxAnimGifCount) {
                    timer.Stop();
                    animated_gifs.GetAt(currentGifIndex - 1).Stop();
                    if (numAnimGifs == 0) {
                        gifs_playing = false;
                        currentGifIndex = 0;
                    }
                    setImageSource();
                }
            }
        }
        else {
            timer.Stop();
            setImageSource();
        }
    }

    void  MainWindow::toggleTimer() {
        if (unbox_value<hstring>(Start_Stop().Content()) == L"START") {
            timer.Start();
            Start_Stop().Content(box_value(L"STOP"));
            if (gifs_playing) {
                if (animated_gifs.Size() > 0 && currentGifIndex > 0) {
                    animated_gifs.GetAt(currentGifIndex - 1).Play();
                }
            }
        }
        else {
            timer.Stop();
            Start_Stop().Content(box_value(L"START"));
            if (gifs_playing) {
                if (animated_gifs.Size() > 0 && currentGifIndex > 0) {
                    animated_gifs.GetAt(currentGifIndex - 1).Stop();
                }
            }
        }
    }
}


void winrt::win3_sample_dev_tutorial::implementation::MainWindow::mainWindow_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowSizeChangedEventArgs const& args)
{
    this->setDimensionalValues();
    OutputDebugStringW(L"size changed\n");
}


void winrt::win3_sample_dev_tutorial::implementation::MainWindow::Start_Stop_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
   this->toggleTimer();
}


void winrt::win3_sample_dev_tutorial::implementation::MainWindow::ChooseDirectory_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    //not implemented currently
}
