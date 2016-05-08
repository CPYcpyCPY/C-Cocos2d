using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Popups;

namespace Todos.ViewModels
{
    class TodoItemViewModel
    {
        public ObservableCollection<Models.Todoitem> allItems = new ObservableCollection<Models.Todoitem>();
        public ObservableCollection<Models.Todoitem> AllItems { get { return this.allItems; } }

        public Models.Todoitem selectedItem = default(Models.Todoitem);
        public Models.Todoitem SelectedItem { get { return selectedItem; } set { this.selectedItem = value; } }

        public TodoItemViewModel()
        {
            this.allItems.Add(new Models.Todoitem("123", "123", DateTime.Now));
            this.allItems.Add(new Models.Todoitem("456", "456", DateTime.Now));
        }

        public void AddTodoItem(string title, string description, DateTime dateTime)
        {
            this.allItems.Add(new Models.Todoitem(title, description, dateTime));
            this.selectedItem = null;
        }

        public Models.Todoitem getTodoItemById(string id)
        {
            foreach(Models.Todoitem item in allItems)
            {
                if(item.getId() == id)
                {
                    return item;
                }
            }
            return null;
        }

        public void RemoveTodoItem(string id)
        {
            allItems.Remove(getTodoItemById(id));
            this.selectedItem = null;
        }

        public void UpdateTodoItem(string id, string title, string description, DateTime dateTime)
        {
            Models.Todoitem item = getTodoItemById(id);
            item.title = title;
            int index = allItems.IndexOf(item);
            item.description = description;
            item.dateTime = dateTime;
            allItems.RemoveAt(index);
            allItems.Insert(index, item);
            this.selectedItem = null;
        }

    }
}
