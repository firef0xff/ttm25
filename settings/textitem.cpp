#include "textitem.h"
#include "ui_textitem.h"

TextItem::TextItem(CallBack fun, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextItem),
    mCallback( fun )
{
    ui->setupUi(this);
}

TextItem::~TextItem()
{
    delete ui;
}

void TextItem::on_buttonBox_accepted()
{
    if ( mCallback )
        mCallback( ui->lineEdit->text() );
}
