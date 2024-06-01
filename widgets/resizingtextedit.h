#ifndef RESIZINGTEXTEDIT_H
#define RESIZINGTEXTEDIT_H

#include <QTextEdit>

class ResizingTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit ResizingTextEdit(QWidget *parent = nullptr);

    void updateHeight();

    int getMaxHeight() const;
    void setMaxHeight(int newMaxHeight);

    int getMinHeight() const;
    void setMinHeight(int newMinHeight);

protected:
    bool event(QEvent *e) override;

private:
    int maxHeight;
    int minHeight;

};

#endif // RESIZINGTEXTEDIT_H
