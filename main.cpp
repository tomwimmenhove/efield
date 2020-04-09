#include "mainwindow.h"
#include <QApplication>

#include <memory>

class TestBase
{
public:
    TestBase(std::unique_ptr<TestBase>&& parent = std::unique_ptr<TestBase>(nullptr)) noexcept
        : parent(std::move(parent))
    {

    }

    ~TestBase()
    {

    }

    std::unique_ptr<TestBase> activate(std::unique_ptr<TestBase>&& current)
    {
        return std::move(current);
    }

protected:
    std::unique_ptr<TestBase> parent;
};

int main(int argc, char *argv[])
{
    auto t = std::make_unique<TestBase>();

    for (int i = 0; i < 1000000; i++)
        t = std::move(t->activate(std::move(t)));


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
