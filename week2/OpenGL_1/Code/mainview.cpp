#include "mainview.h"
#include "math.h"
#include "vertex.h"

#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color of the screen to be black on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    Vertex v[36];
    v[0].x = -1; v[1].x = -1; v[2].x = -1; v[3].x = -1;
    v[0].y = -1; v[1].y = -1; v[2].y =  1; v[3].y =  1;
    v[0].z = -1; v[1].z =  1; v[2].z = -1; v[3].z =  1;
    v[0].r =  0; v[1].r =  0; v[2].r =  0; v[3].r =  0;
    v[0].g =  0; v[1].g =  0; v[2].g =  1; v[3].g =  1;
    v[0].b =  0; v[1].b =  1; v[2].b =  0; v[3].b =  1;

    v[4].x =  1; v[5].x =  1; v[6].x =  1; v[7].x =  1;
    v[4].y = -1; v[5].y = -1; v[6].y =  1; v[7].y =  1;
    v[4].z = -1; v[5].z =  1; v[6].z = -1; v[7].z =  1;
    v[4].r =  1; v[5].r =  1; v[6].r =  1; v[7].r =  1;
    v[4].g =  0; v[5].g =  0; v[6].g =  1; v[7].g =  1;
    v[4].b =  0; v[5].b =  1; v[6].b =  0; v[7].b =  1;

    v[0].x = -1; v[1].x = -1; v[2].x =  1;
    v[0].y = -1; v[1].y = -1; v[2].y = -1;
    v[0].z = -1; v[1].z =  1; v[2].z = -1;
    v[0].r =  0; v[1].r =  0; v[2].r =  0;
    v[0].g =  0; v[1].g =  0; v[2].g =  1;
    v[0].b =  0; v[1].b =  1; v[2].b =  0;

    v[0].x =  1; v[1].x =  1; v[2].x = -1;
    v[0].y = -1; v[1].y = -1; v[2].y = -1;
    v[0].z = -1; v[1].z =  1; v[2].z =  1;
    v[0].r =  0; v[1].r =  0; v[2].r =  0;
    v[0].g =  0; v[1].g =  0; v[2].g =  1;
    v[0].b =  0; v[1].b =  1; v[2].b =  0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(vbo, sizeof(v), v, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 3, GL_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float)));


}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    // Draw here
    glDrawArrays(GL_TRIANGLES, 0, 8);

    shaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) 
{
    // TODO: Update projection to fit the new aspect ratio
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    Q_UNIMPLEMENTED();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;
    Q_UNIMPLEMENTED();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
