/*
Copyright Copyright 2016-17 Sacha Schutz

    This file is part of FastQt.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

    @author : Pierre Lindenbaum from FastQC <http://www.bioinformatics.babraham.ac.uk/projects/fastqc/>
    @author : Sacha Schutz <sacha@labsquare.org>
    @author : Pierre Marijon <pierre@marijon.fr>
*/
#include "mainwindow.h"
#include "qfonticon.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    mView = new MainAnalyseView;
    mStatusBar = new QStatusBar;
    setCentralWidget(mView);

    setStatusBar(mStatusBar);

    setupActions();
    resize(800,600);

    move(200,200);


}

MainWindow::~MainWindow()
{

}

void MainWindow::addFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,tr("Open Fastq file"), QDir::homePath(), tr("Fastq Files (*.fastq *.fastq.gz *.fastq.bz2 *.fastq.xz)"));
    if (!fileNames.isEmpty())
    {
        for (QString file : fileNames)
        {
            mView->addFile(file);
        }
    }

    statusBar()->showMessage(QString(tr("Running on %1 threads")).arg(QThreadPool::globalInstance()->activeThreadCount()));

}

void MainWindow::remFiles()
{
    int ret=QMessageBox::warning(this,tr("Warning"),
                                 tr("Are you sure you want to delete selected analysis ?"),
                                 QMessageBox::Yes|QMessageBox::No );

    if (ret == QMessageBox::Yes )
        mView->removeSelection();

}

void MainWindow::stopFiles()
{
    int ret=QMessageBox::warning(this,tr("Warning"),
                                 tr("Are you sure you want to stop selected analysis ?"),
                                 QMessageBox::Yes|QMessageBox::No );

    if (ret == QMessageBox::Yes )
        mView->stopSelection();

}

void MainWindow::clearFiles()
{
    int ret=QMessageBox::warning(this,tr("Warning"),
                                 tr("Are you sure you want to remove all analysis?"),
                                 QMessageBox::Yes|QMessageBox::No );

    if (ret == QMessageBox::Yes )
        mView->clearAll();
}

void MainWindow::showAnalysis()
{
    auto cIndex = mView->currentIndex();
    if (cIndex.isValid())
        mView->showAnalysis(cIndex);
}

void MainWindow::run()
{

    //    addFile("/home/sacha/merge.fastq");
    //    mainList.first()->run();

}



void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::exportSelection()
{
    if (mView->selectionModel()->selectedRows().count() <= 0)
    {
        QMessageBox::warning(this,tr("Cannot export"),tr("Your selection is empty"));
        return;
    }

    QString dirname = QFileDialog::getExistingDirectory(this,tr("Select a directory to save results"));
    if (!dirname.isEmpty())
        mView->exportSelection(dirname);
}

void MainWindow::setupActions()
{

    // Create Menu Bar
    setMenuBar(new QMenuBar());
    // File menu
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
    QAction * openAction = fileMenu->addAction(QFontIcon::icon(0xf067), tr("&Add files"),this, SLOT(addFiles()), QKeySequence::Open);
    QAction * exportSelAction = fileMenu->addAction(QFontIcon::icon(0xf0c7),tr("&Export selection"),this, SLOT(exportSelection()), QKeySequence::Save);

    fileMenu->addSeparator();
    fileMenu->addAction(QFontIcon::icon(0xf00d),tr("&Close"),qApp, SLOT(closeAllWindows()), QKeySequence::Close);


    //Edit menu
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));
    QAction * remAction  = editMenu->addAction(QFontIcon::icon(0xf068), tr("&Remove"),this, SLOT(remFiles()), QKeySequence::Delete);
    QAction * stopAction = editMenu->addAction(QFontIcon::icon(0xf04d), tr("&Stop"),this, SLOT(stopFiles()));
    QAction * clearAction= editMenu->addAction(QFontIcon::icon(0xf1f8), tr("&Clear all"),this, SLOT(clearFiles()));
    editMenu->addSeparator();
    editMenu->addAction(tr("&Select all"),mView,SLOT(selectAll()), QKeySequence::SelectAll);


    //View menu
    QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
    QAction * showAction = viewMenu->addAction(QFontIcon::icon(0xf06e), tr("&Show analysis"),this, SLOT(showAnalysis()));




    // Help menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(QFontIcon::icon(0xf129),tr("About %1").arg(qApp->applicationName()),this,SLOT(about()));
    helpMenu->addAction(QFontIcon::icon(0xf129),tr("About &Qt"),qApp, SLOT(aboutQt()));


    QToolBar * bar = addToolBar(tr("Open"));
    bar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    bar->addAction(openAction);
    bar->addAction(remAction);
    bar->addSeparator();
    bar->addAction(exportSelAction);
    bar->addSeparator();
    bar->addAction(stopAction);
    bar->addAction(clearAction);
    bar->addSeparator();
    bar->addAction(showAction);



}



