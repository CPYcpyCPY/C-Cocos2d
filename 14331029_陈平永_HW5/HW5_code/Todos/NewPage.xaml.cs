using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Popups;
using Windows.Storage.Pickers;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Media.Imaging;
using Todos;

namespace Todos
{
    public sealed partial class NewPage : Page
    {
        private int valid;
        private string curId;
        MainPage rootPage = MainPage.Current;
        public NewPage()
        {
            this.InitializeComponent();
            var viewTitleBar = Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().TitleBar;
            viewTitleBar.BackgroundColor = Windows.UI.Colors.CornflowerBlue;
            viewTitleBar.ButtonBackgroundColor = Windows.UI.Colors.CornflowerBlue;
        }

        private ViewModels.TodoItemViewModel ViewModel;

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ViewModel = ((ViewModels.TodoItemViewModel)e.Parameter);
            if(ViewModel.SelectedItem == null)
            {
                btnCreateOrUpdate.Content = "Create";
                title.Text = "";
                detail.Text = "";
                itemDelete.Visibility = Visibility.Collapsed;
            } else
            {
                btnCreateOrUpdate.Content = "Update";
                curId = ViewModel.SelectedItem.getId();
                title.Text = ViewModel.SelectedItem.title;
                detail.Text = ViewModel.SelectedItem.description;
                datePicker.Date = ViewModel.SelectedItem.dateTime;
                itemDelete.Visibility = Visibility.Visible;
            }
            Frame rootFrame = Window.Current.Content as Frame;
            if (rootFrame.CanGoBack)
            {
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                    AppViewBackButtonVisibility.Visible;
            }
            else
            {
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                    AppViewBackButtonVisibility.Collapsed;
            }
        }

        private void createOrUpdate(object sender, RoutedEventArgs e)
        {
            if (valid == 1)
            {
                var i = new MessageDialog("请选择正确的日期！").ShowAsync();
                return;
            } else if(btnCreateOrUpdate.Content.ToString() == "Update")
            {
                ViewModel.UpdateTodoItem(curId, title.Text, detail.Text, datePicker.Date.DateTime);
                Frame.Navigate(typeof(MainPage), ViewModel);
                ViewModel.SelectedItem = null;
            } else
            {
                ViewModel.AddTodoItem(title.Text, detail.Text, datePicker.Date.DateTime);
                Frame.Navigate(typeof(MainPage), ViewModel);
                ViewModel.SelectedItem = null;
            }
        }

        private void cancelDate(object sender, RoutedEventArgs e)
        {
            title.Text = "";
            detail.Text = "";
            datePicker.Date = DateTime.Now.Date;
        }

        private void DatePicker_DateChanged(object sender, DatePickerValueChangedEventArgs e)
        {
            DateTime selectDate = e.NewDate.DateTime;
            string str1 = DateTime.Now.ToString().Substring(0, 9);
            string str2 = e.NewDate.DateTime.ToString().Substring(0, 9);
            if(str1 == str2)
            {
                valid = 0;
            } else
            {
                valid = DateTime.Now.CompareTo(selectDate);
            }
        }

        private async void selectImage(object sender, RoutedEventArgs e)
        {
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            openPicker.FileTypeFilter.Add(".jpg");
            openPicker.FileTypeFilter.Add(".jpeg");
            openPicker.FileTypeFilter.Add(".png");
            StorageFile file = await openPicker.PickSingleFileAsync();
            if(file != null)
            {
                using (IRandomAccessStream fileStream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read))
                {
                    // Set the image source to the selected bitmap 
                    BitmapImage bitmapImage = new BitmapImage();
                    await bitmapImage.SetSourceAsync(fileStream);
                    Image.Source = bitmapImage;
                }
            }
        }

        private void btnDelete(object sender, RoutedEventArgs e)
        {
            ViewModel.RemoveTodoItem(curId);
            Frame.Navigate(typeof(MainPage), ViewModel);
        }

    }
}
