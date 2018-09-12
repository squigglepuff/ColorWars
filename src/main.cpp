#include "include/mainwindow.h"
#include <QApplication>

std::map<ECellColors, QString> g_ColorNameMap;

void SetupColorNames()
{
    g_ColorNameMap[Cell_White] = "White";
    g_ColorNameMap[Cell_Red] = "Red";
    g_ColorNameMap[Cell_Orange] = "Orange";
    g_ColorNameMap[Cell_Yellow] = "Yellow";
    g_ColorNameMap[Cell_Lime] = "Lime";
    g_ColorNameMap[Cell_Green] = "Green";
    g_ColorNameMap[Cell_Cyan] = "Cyan";
    g_ColorNameMap[Cell_Blue] = "Blue";
    g_ColorNameMap[Cell_Purple] = "Purple";
    g_ColorNameMap[Cell_Magenta] = "Magenta";
    g_ColorNameMap[Cell_Pink] = "Pink";
    g_ColorNameMap[Cell_Brown] = "Brown";
    g_ColorNameMap[Cell_Gray] = "Gray";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMainWindow w;
    w.show();

    return a.exec();
}
