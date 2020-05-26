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

    void on_actionTest_triggered();
    void on_actionOpen_triggered();
    void on_actionExport_triggered();
    void on_action_repl_triggered();
    void on_collapse_quiz_triggered();
    void on_action_find_triggered();
    void on_action_next_triggered();
    void on_tree_itemDoubleClicked(QTreeWidgetItem * item, int column);
    void on_tree_itemSelectionChanged();
    void on_action_textToNum_triggered();
    void on_action_fromBilet_triggered();
    void on_actionFixTolerance_triggered(bool checked);

  private:
    Ui::MainWindow * ui;

    QString last_dir;
    QSettings * sett;
    QString search;
    QLabel * infolabel;
    QStringList images;
    QString imgsrcpath;
    QLineEdit * tolerance;
    MyHighlighter * highlighter;
    void writeText(QXmlStreamWriter & stream, QString txt, QString basepath);
};    // class MainWindow


#endif    // MAINWINDOW_H
