#include "mainwindow.h"
#include <QApplication>


#include <memory>

template<typename U>
class ContainerIter
{
public:
    ContainerIter(std::vector<std::unique_ptr<int>>::iterator i)
        : i(i)
    { }

    bool operator ==(const ContainerIter& other) { return i == other.i; }
    bool operator !=(const ContainerIter& other) { return i != other.i; }

    U& operator *() { return **i; }
    ContainerIter & operator++() { ++i; return *this; }
    ContainerIter operator++(int)
    {
        ContainerIter tmp = *this;
        i++;
        return tmp;
    }

private:
    std::vector<std::unique_ptr<int>>::iterator i;
};

class Container
{
public:
    ContainerIter<int> begin() { return ContainerIter<int>(elements.begin()); }
    ContainerIter<int> end() { return ContainerIter<int>(elements.end()); }

    ContainerIter<const int> cbegin() { return ContainerIter<const int>(elements.begin()); }
    ContainerIter<const int> cend() { return ContainerIter<const int>(elements.end()); }



    void Add(std::unique_ptr<int>&& element)
    {
        elements.push_back(std::move(element));
    }

private:
    std::vector<std::unique_ptr<int>> elements;
};

int main(int argc, char *argv[])
{
    Container c;

    c.Add(std::make_unique<int>(42));
    c.Add(std::make_unique<int>(42));
    c.Add(std::make_unique<int>(43));

    *c.begin() = 100;

    for(auto i: c)
    {
        qDebug() << i;
    }

    for(const auto& i: c)
    {
        qDebug() << i;
    }

    //return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
