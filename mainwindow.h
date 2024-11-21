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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    virtual void resizeEvent(QResizeEvent *);

private slots:

    void on_actionAnalyse_triggered();
    void on_actionOpen_triggered();
    void on_actionExport_triggered();
    void on_actionSet_work_dir_triggered();
    void on_actionReplace_triggered();
    void on_actionCollapse_triggered();
    void on_actionFind_triggered();
    void on_actionFindNext_triggered();
    void on_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_tree_itemSelectionChanged();
    void on_actionToNumerical_triggered();
    void on_actionFromTickets_triggered();
    void on_actionFixedAccuracy_triggered(bool checked);
    void on_actionDuplicate_search_triggered();
    void on_actionHighlighter_triggered();
    void on_tree_customContextMenuRequested(const QPoint &pos);
    void on_actionHelp_triggered();
    void on_actionRequirements_triggered();
    void on_actionRemoveNoAnswer_triggered();

    void on_actionMarkers_triggered();

private:

    Ui::MainWindow *ui;
    QString        last_dir;
    QSettings      *iniFile;
    QString        search;
    QLabel         *treePositionLabel;
    QStringList    images;
    QLabel         *tolerance_string;
    QLineEdit      *tolerance;
    MyHighlighter  *highlighter;
    bool           usecase;
    QString        default_question_price;


    QTreeWidgetItem *make_question(QStringList &data, int &index);
    bool parse_answer(QString s, QString &price, QString &text, QString &tolerance, bool number = false);

    void writeText(QXmlStreamWriter &stream, QString txt, QString basepath);

    bool process_tree(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance);

    void write_section(QXmlStreamWriter &stream, QTreeWidgetItem *item) const;
    void write_theme(QXmlStreamWriter &stream, QTreeWidgetItem *item) const;

    bool process_question(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance);
    bool write_info(QXmlStreamWriter &stream, QTreeWidgetItem *item);
    bool write_essay(QXmlStreamWriter &stream, QTreeWidgetItem *item);
    bool write_matching(QXmlStreamWriter &stream, QTreeWidgetItem *item);
    bool write_shortanswer(QXmlStreamWriter &stream, QTreeWidgetItem *item);
    bool write_numerical(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance);
    bool write_multichoice(QXmlStreamWriter &stream, QTreeWidgetItem *item);
    bool write_choice(QXmlStreamWriter &stream, QTreeWidgetItem *item);

    bool write_close(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance);

    QString process_subquestion(QTreeWidgetItem *item, double ktolerance, bool btolerance, bool &ok);
    QString format_info(QTreeWidgetItem *item, bool &ok);
    QString format_matching(QTreeWidgetItem *item, bool &ok);
    QString format_shortanswer(QTreeWidgetItem *item, bool &ok);
    QString format_numerical(QTreeWidgetItem *item, double ktolerance, bool btolerance, bool &ok);
    QString format_multichoice(QTreeWidgetItem *item, bool &ok);
    QString format_choice(QTreeWidgetItem *item, bool &ok);

    void show_error(QTreeWidgetItem *item, QString message);
    void export_error();
    bool is_map(const QStringList &answers) const;
    bool is_answer(QTreeWidgetItem *item) const;
    bool is_question(QTreeWidgetItem *item) const;
    bool is_section(QTreeWidgetItem *item) const;
    void change_section_name(QTreeWidgetItem *item);
    void change_question_type(QTreeWidgetItem *item);
    void edit_question(QTreeWidgetItem *item);
    void edit_price(QTreeWidgetItem *pItem);
}; // class MainWindow


#endif // MAINWINDOW_H
