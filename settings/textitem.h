#pragma once
#include <QDialog>
#include <functional>

namespace Ui {
class TextItem;
}

class TextItem : public QDialog
{
    Q_OBJECT

public:
    typedef std::function< void( QString const& ) > CallBack;
    explicit TextItem( CallBack fun, QWidget *parent = 0);
    ~TextItem();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::TextItem *ui;
    CallBack mCallback;
};
