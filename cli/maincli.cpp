#include "maincli.h"

MainCLI::MainCLI(QCommandLineParser* parser, QObject *parent) : QEventLoop(parent), mParser(parser)
{
    mTimer = new QTimer(this);
    mTimer->start(1000);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(updateInfo()));

}

int MainCLI::exec()
{

    if (mParser->positionalArguments().isEmpty())
    {
        qDebug()<<"No fastq provided";
        quit();
    }

    // Set thread number
    int threadNumber;
    if (mParser->value("threads").isEmpty())
        threadNumber = QThread::idealThreadCount();
    else
        threadNumber = mParser->value("threads").toInt();

    cout<<"Starting Analysis..."<<endl;
    cout<<setw(5)<<left<<"Thread number:"<<threadNumber<<endl;
    cout<<setw(5)<<left<<"File count:"<< mParser->positionalArguments().count()<<endl;

    for(QString filename : mParser->positionalArguments())
        cout<<setw(5)<<left<<"File:"<<filename.toStdString()<<endl;


    // Set max thread number
    QThreadPool::globalInstance()->setMaxThreadCount(threadNumber);

    for(QString filename : mParser->positionalArguments())
    {
        AnalysisRunner* runner = new AnalysisRunner();
        runner->setFilename(filename);


        // ON VA FAIRE UN FACTORY POUR CA POUR QUE LE GUI ET CA , SOIT PAREIL
        LengthDistributionAnalysis* len_dist_ana = new LengthDistributionAnalysis;
        runner->addAnalysis(new BasicStatsAnalysis);
        runner->addAnalysis(new PerBaseQualityAnalysis);
        runner->addAnalysis(new PerSequenceQualityAnalysis);
        runner->addAnalysis(new OverRepresentedSeqsAnalysis);
        runner->addAnalysis(new PerBaseNContentAnalysis);
        runner->addAnalysis(new PerSequenceGCContent);
        runner->addAnalysis(len_dist_ana);
        runner->addAnalysis(new PerBaseContentAnalysis(nullptr, len_dist_ana));

        mRunnerList.append(runner);
        QFileInfo info(filename);
        cout<<"Start Analysis "<<info.fileName().toStdString()<<endl;
        QThreadPool::globalInstance()->start(runner);
    }


    return QEventLoop::exec(); // Loop until all runner has been finished
}

void MainCLI::updateInfo()
{

    for (AnalysisRunner * r : mRunnerList)
    {
        QFileInfo info(r->filename());
        if (r->status() == AnalysisRunner::Running)
            cout<<std::setw(5) << left<<"Analysis "<<info.fileName().toStdString()<<" "<<r->progression()<<" %"<<endl;
    }

    if (checkFinish()){

        saveResult();
        quit();
    }

}

void MainCLI::saveResult()
{
    QString path   = mParser->value("outdir");

    if (path.isEmpty())
    {
        QFileInfo info(mRunnerList.first()->filename());
        path = info.dir().path();
    }

    cout<<"Save results in "<< path.toStdString()<<endl;

    for (AnalysisRunner * r : mRunnerList)
    {
        QDir dir(path);
        r->saveAllResult(dir.path());

    }


}

bool MainCLI::checkFinish()
{

    for (AnalysisRunner * r : mRunnerList)
    {
        if (r->status() == AnalysisRunner::Running)
            return false;
    }

    return true;
}
