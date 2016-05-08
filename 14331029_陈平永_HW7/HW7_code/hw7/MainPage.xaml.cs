using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace hw7
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {

        public MainPage()
        {
            this.InitializeComponent();
        }

        private async void GetPhone()
        {
            try
            {   
                //指定所需的url和param
                string url = "http://apis.baidu.com/apistore/mobilephoneservice/mobilephone";
                string param = "tel=" + number.Text;

                //调用request方法
                requestPhone(url, param);

            }
            //有异常则弹出错误提示
            catch (HttpRequestException ex1)
            {
                var i = new MessageDialog(ex1.ToString()).ShowAsync();
            }
            catch (Exception ex2)
            {
                var i = new MessageDialog(ex2.ToString()).ShowAsync();
            }
        }

        
        public async void requestPhone(string url, string param)
        {   
            //组合生产最终的URL
            string strURL = url + '?' + param;

            //声明一个request变量，调用Create方法
            HttpWebRequest request;
            request = (HttpWebRequest)WebRequest.Create(strURL);

            //指明方法为"Get",并添加一个自己的apikey
            request.Method = "GET";
            request.Headers["apikey"] = "4f8deca043fe83cbbea2c1415bb9c13d";

            //声明一个response变量，用于得到返回的数据
            HttpWebResponse response;
            response = (HttpWebResponse) await request.GetResponseAsync();

            Stream s;
            s = response.GetResponseStream();
            string strData = "";
            string strValue = "";

            //得到json字符串
            StreamReader reader = new StreamReader(s, Encoding.UTF8);
            while ((strData = reader.ReadLine()) != null)
            {
                strValue += strData + "\r\n";
            }

            //将得到的json字符串转化为json
            JsonTextReader json1 = new JsonTextReader(new StringReader(strValue));
            string flag = "";

            //读取json中的信息，并显示页面上
            while (json1.Read())
            {
                flag += json1.Value;
                
                //如果手机号不存在，弹出相应的提示
                if(flag.Equals("errNum")) {
                    json1.Read();
                    if(json1.Value.ToString() == "-1")
                    {
                        var mes = new MessageDialog("对不起，您输入的手机号不存在").ShowAsync();
                        break;
                    }
                }

                if(flag.Equals("province"))
                {
                    json1.Read();
                    if(json1.Value.ToString() == "")
                    {
                        var mes = new MessageDialog("对不起，您输入的手机号不存在").ShowAsync();
                        break;
                    }
                    region.Text += json1.Value;
                }
                if(flag.Equals("city"))
                {
                    json1.Read();
                    region.Text += '/';
                    region.Text += json1.Value;
                }
                if(flag.Equals("carrier"))
                {
                    json1.Read();
                    carrier.Text += json1.Value;
                }
                flag = "";
            }

        }


        private static HttpWebResponse await(HttpWebResponse httpWebResponse)
        {
            throw new NotImplementedException();
        }

        private void btn_phone_search(object sender, RoutedEventArgs e)
        {   
            //一开始先情况记录
            region.Text = "";
            carrier.Text = "";
            GetPhone();
        }

    }

    

}
