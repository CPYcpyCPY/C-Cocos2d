using NotificationsExtensions.Tiles;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Todos.Models
{
    class Tile
    {
        public TileContent content { get; set; }
        Models.Todoitem item { get; set; }

        //构造函数，传递Todoitem
        public Tile(ObservableCollection<Models.Todoitem> allitems)
        {
            //获取最新的一个item
            item = allitems.Last();

            content = new TileContent()
            {   
                //设置各种状态下的试图
                Visual = new TileVisual()
                {
                    TileSmall = tileBinding(),
                    TileMedium = tileBinding(),
                    TileWide = tileBinding(),
                    TileLarge = tileBinding()
                }
            };
        }

        private TileBinding tileBinding()
        {
            return new TileBinding()
            {
                Content = new TileBindingContentAdaptive()
                {

                    Children =
                    {
                        new TileText()
                        {
                            Text = item.title,
                            Style = TileTextStyle.SubtitleSubtle
                        },

                        new TileText()
                        {
                            Text = item.description,
                            Style = TileTextStyle.BaseSubtle
                        },

                        new TileText()
                        {
                            Text = item.dateTime.ToString(),
                            Style = TileTextStyle.CaptionSubtle
                        }
                    },
                    PeekImage = new TilePeekImage()
                    {
                        Source = new TileImageSource("Assets/saber.jpg"),
                        Overlay = 8
                    }
                },
            };
        }

    }
}
