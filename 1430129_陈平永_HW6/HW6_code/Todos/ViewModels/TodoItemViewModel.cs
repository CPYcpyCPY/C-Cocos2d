using SQLitePCL;
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

        public static SQLiteConnection conn;

        public TodoItemViewModel()
        {
            //初始化两个Todoitem
            this.allItems.Add(new Models.Todoitem(1, "123", "321", DateTime.Now));
            this.allItems.Add(new Models.Todoitem(2, "456", "654", DateTime.Now));

            initializeDB();
        }

        private void initializeDB()
        {
            //将两个Todoitem的信息存入数据库
            conn = new SQLiteConnection("item.db");

            //创建Items表
            string sql = @"CREATE TABLE IF NOT EXISTS
                                Items (Id     INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
                                       Title   VARCHAR(100),
                                       Detail VARCHAR(1000),
                                       Date   VARCHAR(100)
                           );";

            //将初始化的数据存入表中
            using (var statement = conn.Prepare(sql))
            {
                statement.Step();
                for (int i = 0; i < AllItems.Count; i++)
                {
                    var theItem = AllItems.ElementAt(i);
                    using (var item = conn.Prepare("INSERT INTO Items (Id, Title, Detail, Date) VALUES (?, ?, ?, ?)"))
                    {   
                        item.Bind(1, i + 1);
                        item.Bind(2, theItem.title);
                        item.Bind(3, theItem.description);
                        item.Bind(4, theItem.dateTime.ToString());
                        item.Step();
                    }
                }
            }
        }

        public void AddTodoItem(int db_id, string title, string description, DateTime dateTime)
        {
            this.allItems.Add(new Models.Todoitem(db_id, title, description, dateTime));
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
