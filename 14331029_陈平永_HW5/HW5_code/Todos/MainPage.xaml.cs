using Todos;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Popups;
using Todos.Models;
using NotificationsExtensions.Tiles;
using Windows.UI.Notifications;
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage.Streams;
//using h2;

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace Todos
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public static MainPage Current { get; internal set; }
        private int valid = 0;
        private string curId;

        public MainPage()
        {
            this.InitializeComponent();
            var viewTitleBar = Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().TitleBar;
            viewTitleBar.BackgroundColor = Windows.UI.Colors.CornflowerBlue;
            viewTitleBar.ButtonBackgroundColor = Windows.UI.Colors.CornflowerBlue;
            this.ViewModel = new ViewModels.TodoItemViewModel();
            NavigationCacheMode = NavigationCacheMode.Enabled;
        }

        ViewModels.TodoItemViewModel ViewModel { get; set; }
        DataTransferManager dataTransferManager;

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Frame rootFrame = Window.Current.Content as Frame;
            if (rootFrame.CanGoBack)
            {
                // Show UI in title bar if opted-in and in-app backstack is not empty.
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                    AppViewBackButtonVisibility.Visible;
            }
            else
            {
                // Remove the UI from the title bar if in-app back stack is empty.
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                    AppViewBackButtonVisibility.Collapsed;
            }

            if (e.Parameter.GetType() == typeof(ViewModels.TodoItemViewModel))
            {
                this.ViewModel = (ViewModels.TodoItemViewModel)(e.Parameter);
            }

            dataTransferManager = DataTransferManager.GetForCurrentView();
            dataTransferManager.DataRequested += dataRequested;
        }

        private void dataRequested(DataTransferManager sender, DataRequestedEventArgs args)
        {
            DataRequest request = args.Request;
            Models.Todoitem curItem = ViewModel.getTodoItemById(curId);
            request.Data.Properties.Title = curItem.title;
            request.Data.Properties.Description = curItem.description;
            request.Data.SetData("Date", curItem.dateTime.ToString());
            request.Data.SetText(curItem.description);
        }

        private void btnNewPage(object sender, RoutedEventArgs e)
        {
            if (InlineToDoItemViewGrid.Visibility.ToString() == "Visible") return;
            Frame.Navigate(typeof(NewPage), ViewModel);
        }

        private void TodoItem_ItemClicked(object sender, ItemClickEventArgs e)
        {
            ViewModel.SelectedItem = (Models.Todoitem)(e.ClickedItem);
            Frame.Navigate(typeof(NewPage), ViewModel);

        }

        private void cancel(object sender, RoutedEventArgs e)
        {
            title.Text = "";
            detail.Text = "";
            datePicker.Date = DateTime.Now.Date;
        }

        private void create(object sender, RoutedEventArgs e)
        {
            if (valid == 1)
            {
                var i = new MessageDialog("请选择正确的日期！").ShowAsync();
                return;
            } else
            {
                ViewModel.AddTodoItem(title.Text, detail.Text, datePicker.Date.DateTime);
                ViewModel.SelectedItem = null;
            }

        }

        private void DatePicker_DateChanged(object sender, DatePickerValueChangedEventArgs e)
        {
            DateTime selectDate = e.NewDate.DateTime;
            string str1 = DateTime.Now.ToString().Substring(0, 9);
            string str2 = e.NewDate.DateTime.ToString().Substring(0, 9);
            if (str1 == str2)
            {
                valid = 0;
            }
            else
            {
                valid = DateTime.Now.CompareTo(selectDate);
            }
        }

        private void menuEdit(object sender, RoutedEventArgs e)
        {
            MenuFlyoutItem menuDeleteItem = sender as MenuFlyoutItem;
            curId = menuDeleteItem.DataContext.ToString();
            ViewModel.SelectedItem = ViewModel.getTodoItemById(curId);
            Frame.Navigate(typeof(NewPage), ViewModel);
        }

        private void menuDelete(object sender, RoutedEventArgs e)
        {
            MenuFlyoutItem menuDeleteItem = sender as MenuFlyoutItem;
            curId = menuDeleteItem.DataContext.ToString();
            ViewModel.RemoveTodoItem(curId);
        }

        private void changeTile(object sender, RoutedEventArgs e)
        {   
            //如果当前没有item,则不做处理
            if (ViewModel.AllItems.Count == 0) return;

            Tile tile = new Tile(ViewModel.AllItems);
            TileContent content = tile.content;

            var updatort = TileUpdateManager.CreateTileUpdaterForApplication();
            var notif = new TileNotification(content.GetXml());
            updatort.Update(notif);
        }

        private void itemShare(object sender, RoutedEventArgs e)
        {
            MenuFlyoutItem menuShareItem = sender as MenuFlyoutItem;
            curId = menuShareItem.DataContext.ToString();
            DataTransferManager.ShowShareUI();
        }

    }
}