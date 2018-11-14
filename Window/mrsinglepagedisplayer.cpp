#include "mrsinglepagedisplayer.h"
#include "mrarticleloader.h"
#include <QDebug>
#include <QScrollBar>
MRSinglePageDisplayer::MRSinglePageDisplayer(QWidget *parent) : MRArticleDisplayer(parent)
{
    loadUI ();
    loadSignals ();
}

void MRSinglePageDisplayer::loadUI()
{
    MRArticleDisplayer::loadUI ();
    page = new MRArticlePageDisplayer(this);
    setWidget (page);
}

void MRSinglePageDisplayer::loadSignals()
{
    connect (this, &MRArticleDisplayer::cursorType,
             page, &MRArticlePageDisplayer::handleCursorType);
    connect (this, &MRArticleDisplayer::colorReady,
             page, &MRArticlePageDisplayer::updateColor);
    loader = new MRArticleLoader(this);
    connect (loader, &MRArticleLoader::docReady,
             this, &MRSinglePageDisplayer::handleDocReady);
    connect (loader, &MRArticleLoader::indexReady,
             this, &MRArticleDisplayer::ADCIndexItemsReady);
    render = new ArticlePageRender(this);
    connect (render, &ArticlePageRender::pageReady,
             this, &MRSinglePageDisplayer::displayPage);
//    connect (verticalScrollBar (), &QScrollBar::valueChanged,
//             this, &MRArticleDisplayer::handleScrollBarChanged);
    render->start ();
}

void MRSinglePageDisplayer::handleDocReady(bool ret, std::shared_ptr<MRDocument> document)
{
    if(ret)
    {
        doc = document;
        render->setPDFDocument (doc);
        render->requestPage (0, 1.0, 1.0, 0.0);
    }
    else
    {
        qDebug() << "can not open file";
    }
}

void MRSinglePageDisplayer::displayFirstPage()
{
    Q_ASSERT (page);
    int current = 0;
    render->requestPage (current, 1.0, 1.0, 0.0);
}

void MRSinglePageDisplayer::displayPrevPage()
{
    Q_ASSERT (page != nullptr);
    int current = page->pageIndex ();
    float scaleX = page->getScaleX ();
    float scaleY = page->getScaleY ();
    if(current > 0)
        render->requestPage (--current, scaleX, scaleY, 0.0);
}

void MRSinglePageDisplayer::displayNextPage()
{
    Q_ASSERT (page != nullptr );
    int current = page->pageIndex ();
    float scaleX = page->getScaleX ();
    float scaleY = page->getScaleY ();
    if(current < (doc->pageCount ()-1))
        render->requestPage (++current, scaleX, scaleY, 0.0);
}

void MRSinglePageDisplayer::displayLastPage()
{
    Q_ASSERT (page != nullptr );
    int current = doc->pageCount ();
    float scaleX = page->getScaleX ();
    float scaleY = page->getScaleY ();
    render->requestPage (current-1, scaleX, scaleY, 0.0);
}

void MRSinglePageDisplayer::zoomInPage()
{
    Q_ASSERT (page != nullptr );
    int current = page->pageIndex ();
    float scaleX = page->getScaleX ();
    float scaleY = page->getScaleY ();
    scaleX += 0.2;
    scaleY += 0.2;
    render->requestPage (current, scaleX, scaleY, 0.0);
}

void MRSinglePageDisplayer::zoomOutPage()
{
    Q_ASSERT (page != nullptr );
    int current = page->pageIndex ();
    float scaleX = page->getScaleX ();
    float scaleY = page->getScaleY ();
    scaleX -= 0.2;
    scaleY -= 0.2;
    if(scaleX <= 0.2 || scaleY <= 0.2)
        return;
    render->requestPage (current, scaleX, scaleY, 0.0);
}

void MRSinglePageDisplayer::displayPage(float scaleX, float scaleY, float rotation, int index, QImage img, std::shared_ptr<MRPage> src)
{
    Q_ASSERT (page != nullptr);
    current = index;
    page->displayPage (scaleX, scaleY, rotation, index, img, src);
    verticalScrollBar ()->setValue (0);

}