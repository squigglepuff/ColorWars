#include "include/mainwindow.h"
#include <QApplication>

std::map<ECellColors, std::string> g_ColorNameMap;

void SetupColorNames()
{
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_White, "White"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Red, "Red"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Orange, "Orange"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Yellow, "Yellow"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Lime, "Lime"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Green, "Green"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Cyan, "Cyan"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Blue, "Blue"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Purple, "Purple"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Magenta, "Magenta"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Pink, "Pink"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Brown, "Brown"));
    g_ColorNameMap.insert(std::map<ECellColors,std::string>(Cell_Gray, "Gray"));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMainWindow w;
    w.show();

    return a.exec();
}
