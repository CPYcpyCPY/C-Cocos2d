using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Shapes;

namespace Todos.Models
{
    class Todoitem
    {
        public string id;
        public string title { get; set; }
        public string description { get; set; }
        public bool? completed { get; set; }
        public string getId() { return id; }
        public DateTime dateTime { get; set; }

        public Todoitem(string title, string description, DateTime dateTime)
        {
            this.id = Guid.NewGuid().ToString();
            this.title = title;
            this.description = description;
            this.completed = true;
            this.dateTime = dateTime;
        }
    }
}
