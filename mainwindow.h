#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QtCore/QXmlStreamWriter>
namespace Ui
{
class MainWindow;
}
class QTreeWidgetItem;
class QLabel;
class QLineEdit;
class MyHighlighter;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget * parent = 0);

    ~MainWindow();

    virtual void resizeEvent(QResizeEvent *);

  private slots:

    void on_actionAnalyse_triggered();
    void on_actionOpen_triggered();
    void on_actionExport_triggered();
    void on_actionReplace_triggered();
    void on_actionCollapse_triggered();
    void on_actionFind_triggered();
    void on_actionFindNext_triggered();
    void on_tree_itemDoubleClicked(QTreeWidgetItem * item, int column);
    void on_tree_itemSelectionChanged();
    void on_actionToNumerical_triggered();
    void on_actionFromTickets_triggered();
    void on_actionFixedAccuracy_triggered(bool checked);

  private:
    Ui::MainWindow * ui;
    QString last_dir;
    QSettings * iniFile;
    QString search;
    QLabel * treePositionLabel;
    QStringList images;
    QLineEdit * tolerance;
    MyHighlighter * highlighter;
    void writeText(QXmlStreamWriter & stream, QString txt, QString basepath);
};    // class MainWindow


#endif    // MAINWINDOW_H
