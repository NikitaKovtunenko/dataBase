#include <iostream>
#include <stdarg.h>
#include <mutex>
#include <map>
#include <memory>
#include <thread>

struct Student{
    int id;
    std::string name;
    unsigned int age;
};


class Database{
    std::mutex mtx;
    std::map<int, std::shared_ptr<Student>> _data;
    Database(){}
public:
    Database(const Database&) = delete;
    Database& operator = (const Database&) = delete;

    static Database* instance(){
        static Database base;
        return &base;
    }

    void addToBase(const Student& student){
        auto it = _data.find(student.id);

        if(it != _data.end())
            throw std::runtime_error("The student is already in the database");

        std::lock_guard<std::mutex> lock(mtx);
        _data[student.id] = std::make_shared<Student>(student);
    }

    void deleteStudent(const int id)
    {
        if(_data.empty())
            throw std::runtime_error("Database is empty!");

        std::lock_guard<std::mutex> lock(mtx);
        if(_data.erase(id) == 0)
            throw std::runtime_error("The student is not on the list.");

    }

    void studentInfo(const int& id){

        if(_data.empty())
            throw std::runtime_error("Database is empty!");

        std::lock_guard<std::mutex> lock(mtx);
        auto it = _data.find(id);

        if(it == _data.end())
            throw std::runtime_error("The student is not on the list.");

        std::cout << "ID " << it->second->id << std::endl;
        std::cout << "Name " << it->second->name<< std::endl;
        std::cout << "Age " << it->second->age<< std::endl;
    }




};

void addToDatabase_handler()
{
    auto dataBase = Database::instance();
    Student st1{1,"Ivan",18};
    Student st2{2,"Nikita",19};
    Student st3{3,"Nikolay",20};
    Student st4{4,"Vladimir",21};
    Student st5{5,"Kirill",22};


    try {
        dataBase->addToBase(st1);
        dataBase->addToBase(st2);
        dataBase->addToBase(st3);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        dataBase->addToBase(st4);
        dataBase->addToBase(st5);

        auto dataBase2 = Database::instance();
        dataBase2->studentInfo(5);
        dataBase2->studentInfo(4);
        dataBase2->studentInfo(3);
        dataBase2->studentInfo(2);
        dataBase2->studentInfo(1);
    }catch (std::runtime_error& er)
    {
        std::cerr << er.what() << std::endl;
    }
}

void deleteStudentDatabase_handler(){
  //  std::this_thread::sleep_for(std::chrono::milliseconds(20));
    auto dataBase = Database::instance();

    try {
        dataBase->deleteStudent(5);
        dataBase->deleteStudent(1);
        dataBase->deleteStudent(3);
        dataBase->deleteStudent(4);
        dataBase->deleteStudent(2);
    }catch (std::runtime_error& er)
    {
        std::cerr << er.what() << std::endl;
    }

}

int main() {
    std::thread t1(addToDatabase_handler);
    std::thread t2(deleteStudentDatabase_handler);

    t1.join();
    t2.join();
    return 0;
}
