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
using SQLitePCL;
using System.Text;

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
        public static int db_id;
        public static int deleteNum = 0;

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
        }

        private void btnNewPage(object sender, RoutedEventArgs e)
        {
            if (InlineToDoItemViewGrid.Visibility.ToString() == "Visible") return;
            Frame.Navigate(typeof(NewPage), ViewModel);
        }

        private void TodoItem_ItemClicked(object sender, ItemClickEventArgs e)
        {
            ViewModel.SelectedItem = (Models.Todoitem)(e.ClickedItem);
            db_id = ViewModel.AllItems.IndexOf((Models.Todoitem)(e.ClickedItem)) + 1;
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
                ViewModel.AddTodoItem(ViewModel.AllItems.Count + 1, title.Text, detail.Text, datePicker.Date.DateTime);

                var db = ViewModels.TodoItemViewModel.conn;

                using (var item = db.Prepare("INSERT INTO Items (Id, Title, Detail, Date) VALUES (?, ?, ?, ?)"))
                {
                    item.Bind(1, ViewModel.AllItems.Count + 1);
                    item.Bind(2, title.Text);
                    item.Bind(3, detail.Text);
                    item.Bind(4, datePicker.Date.DateTime.ToString());
                    item.Step();
                }

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

        //item的Edit事件
        private void menuEdit(object sender, RoutedEventArgs e)
        {
            MenuFlyoutItem menuEditItem = sender as MenuFlyoutItem;
            curId = menuEditItem.DataContext.ToString();
            ViewModel.SelectedItem = ViewModel.getTodoItemById(curId);

            db_id = menuEditItem.TabIndex;
            Frame.Navigate(typeof(NewPage), ViewModel);
        }

        //item的Delete事件
        private void menuDelete(object sender, RoutedEventArgs e)
        {
            MenuFlyoutItem menuDeleteItem = sender as MenuFlyoutItem;
            curId = menuDeleteItem.DataContext.ToString();
            ViewModel.RemoveTodoItem(curId);

            db_id = menuDeleteItem.TabIndex;
            var db = ViewModels.TodoItemViewModel.conn;

            using (var item = db.Prepare("DELETE FROM Items WHERE Id = ?"))
            {
                item.Bind(1, db_id);
                item.Step();
            }

        }

        //数据库查询函数
        private void btnSearch(object sender, RoutedEventArgs e)
        {
            var db = ViewModels.TodoItemViewModel.conn;
            string searchInfo = searchBox.Text;
            searchInfo = searchInfo.Insert(0, "%");
            searchInfo = searchInfo.Insert(searchInfo.Length, "%");
            using (var searchItem = db.Prepare("SELECT * FROM Items WHERE Title LIKE ? or Detail LIKE ? or Date LIKE ?"))
            {
                searchItem.Bind(1, searchInfo);
                searchItem.Bind(2, searchInfo);
                searchItem.Bind(3, searchInfo);
                string result = "";
                while (SQLiteResult.ROW == searchItem.Step())
                {
                    string info = "Id:" + searchItem[0] + "; " +
                                  "Title:" + searchItem[1] + "; " +
                                  "Detail:" + searchItem[2] + "; " +
                                  "Date:" + searchItem[3] + ";\n";
                    result += info;
                }
                if(result != "")
                {
                    var i = new MessageDialog(result).ShowAsync();
                } else
                {
                    var i = new MessageDialog("无相关的数据").ShowAsync();
                }
               
            };
        }

    }
}