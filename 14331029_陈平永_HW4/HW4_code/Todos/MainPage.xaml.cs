﻿using System;
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

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace Todos
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            var viewTitleBar = Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().TitleBar;
            viewTitleBar.BackgroundColor = Windows.UI.Colors.CornflowerBlue;
            viewTitleBar.ButtonBackgroundColor = Windows.UI.Colors.CornflowerBlue;
            NavigationCacheMode = NavigationCacheMode.Enabled;
        }

        private void AddAppBarButton_Click(object sender, RoutedEventArgs e)
        {
            Frame.Navigate(typeof(NewPage), "");
        }

        private void clickHandler(object sender, RoutedEventArgs e)
        {
            CheckBox checkbox = sender as CheckBox;
            if(checkbox.Name == "checkbox1")
            {
                line1.Visibility = Visibility.Visible;
            } else
            {
                line2.Visibility = Visibility.Visible;
            }
        }

        private void unClickHandler(object sender, RoutedEventArgs e)
        {
            CheckBox checkbox = sender as CheckBox;
            if (checkbox.Name == "checkbox1")
            {
                line1.Visibility = Visibility.Collapsed;
            }
            else
            {
                line2.Visibility = Visibility.Collapsed;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if(e.NavigationMode == NavigationMode.New)
            {
                ApplicationData.Current.LocalSettings.Values.Remove("data1");
            } else
            {
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("data2"))
                {
                    var composite = ApplicationData.Current.LocalSettings.Values["data2"] as ApplicationDataCompositeValue;
                    checkbox1.IsChecked = (bool)composite["isChecked1"];
                    checkbox2.IsChecked = (bool)composite["isChecked2"];
                    if ((string)composite["visibility1"] == "Visible")
                    {
                        line1.Visibility = Visibility.Visible;
                    } else
                    {
                        line1.Visibility = Visibility.Collapsed;
                    }
                    if ((string)composite["visibility2"] == "Visible")
                    {
                        line2.Visibility = Visibility.Visible;
                    }
                    else
                    {
                        line2.Visibility = Visibility.Collapsed;
                    }
                    ApplicationData.Current.LocalSettings.Values.Remove("data2");
                }
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            bool suspending = ((App)App.Current).IsSuspending;
            if (suspending)
            {
                var composite = new ApplicationDataCompositeValue();
                composite["isChecked1"] = checkbox1.IsChecked;
                composite["isChecked2"] = checkbox2.IsChecked;
                composite["visibility1"] = line1.Visibility.ToString();
                composite["visibility2"] = line2.Visibility.ToString();
                ApplicationData.Current.LocalSettings.Values["data2"] = composite;
            }
        }

    }
}
