#pragma once

/*********************************************************
* Author: Navjot S. Sandhu
* Description: Interview Project for Verizon BlueJeans; EPAM Staffing
*              Allows playback of images in a directory at 15 fps, and animated gifs, with each frame being split between 4 sub-images
*              in corners with space horizontally and vertically between
*Date: 9/7/2022
*Technology: WinRT, Window Sdk App using c++
*MainWindow:  Primary Application class with user interface, that loads and displays images
**************************************************/
#include "MainWindow.g.h"
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Storage.Search.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Media::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::win3_sample_dev_tutorial::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow(); //default ctor
        ~MainWindow();

        void OnTick(Windows::Foundation::IInspectable const&,Windows::Foundation::IInspectable const&); //handles timer events

        IAsyncAction MainWindow::LoadFilesAsync(hstring); //loads files, takes filedir name inside Assets folder
        
        void mainWindow_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowSizeChangedEventArgs const& args); //window resize handler
        
        void Start_Stop_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e); //handles start stop button events
        
        void ChooseDirectory_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e); //would let a user choose a directory of images
    
    private:
        DispatcherTimer timer; //timer for fps for still images
        
        void setDimensionalValues(); //sets image dimensional values after resize of window
        
        void setImageSource(); //sets image source to the 4 different image controls
        
        Windows::Foundation::Collections::IVector<WriteableBitmap> images{ nullptr };  //holds bitmaps for images
        Windows::Foundation::Collections::IVector<BitmapImage> animated_gifs{ nullptr }; //holds bitmaps for animated gifs

        //  std::vector<ImageSet> images;
         // Windows::Foundation::Collections::IVector<IRandomAccessStream> streams{nullptr};
        Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile> filesInFolder; //holders files in directory as StorageFile 
        
        hstring imgDir = L"photopack01"; //default image directory
        
        int currentImgIndex; //current image index
        int currentGifIndex; //current animated gif index
        int numAnimGifs; //holds number of animated gifs
        int totalNumImages; //total number of images loaded
        
        int animGifCount = 0;  //animated gif playing attributes 
        bool gifs_playing = false;
        int maxAnimGifCount = 20; //number of tick counts of counter at 15 fps, so play for ~ 200ms
        bool gifPlaying = false;

        void toggleTimer(); //toggle timer and start stop button content for start stopping of timer to display images
       
        int imageWidth; //image size based on window dimensions
        int imageHeight;
    };
}

namespace winrt::win3_sample_dev_tutorial::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
