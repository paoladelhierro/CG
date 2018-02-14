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

    glDeleteBuffers(1,&vbo);
    glDeleteVertexArrays(1,&vao1);
    glDeleteVertexArrays(1,&vao2);
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

    Vertex a, b, c, d, e, f, g, h;
    a.x = -1; b.x =  1; c.x =  1; d.x = -1;
    a.y = -1; b.y = -1; c.y = -1; d.y = -1;
    a.z = -1; b.z = -1; c.z =  1; d.z =  1;
    a.r =  0; b.r =  0; c.r =  0; d.r =  0;
    a.g =  0; b.g =  0; c.g =  1; d.g =  1;
    a.b =  0; b.b =  1; c.b =  0; d.b =  1;

    e.x = -1; f.x =  1; g.x =  1; h.x = -1;
    e.y =  1; f.y =  1; g.y =  1; h.y =  1;
    e.z = -1; f.z = -1; g.z =  1; h.z =  1;
    e.r =  1; f.r =  1; g.r =  1; h.r =  1;
    e.g =  0; f.g =  0; g.g =  1; h.g =  1;
    e.b =  0; f.b =  1; g.b =  0; h.b =  1;


    Vertex vertices1[36] = {a, b, c, // bottom
                           a, c, d,
                           d, c, g, // front
                           d, c, h,
                           b, a, e, // back
                           b, e, f,
                           h, g, f, // top
                           h, f, e,
                           a, d, e, // left
                           d, h, e,
                           c, b, f, // right
                           c, f, g};

    a.x = -1; b.x =  1; c.x =  1; d.x = -1;
    a.y = -1; b.y = -1; c.y = -1; d.y = -1;
    a.z =  1; b.z =  1; c.z = -1; d.z = -1;
    a.r =  1; b.r =  0; c.r =  0; d.r =  0;
    a.g =  1; b.g =  0; c.g =  1; d.g =  1;
    a.b =  0; b.b =  1; c.b =  0; d.b =  1;

    e.x =  0; // f.x =  1; g.x =  1; h.x = -1;
    e.y =  1; // f.y =  1; g.y =  1; h.y =  1;
    e.z =  0; // f.z = -1; g.z =  1; h.z =  1;
    e.r =  1; // f.r =  1; g.r =  1; h.r =  1;
    e.g =  0; // f.g =  0; g.g =  1; h.g =  1;
    e.b =  0; // f.b =  1; g.b =  0; h.b =  1;

    Vertex vertices2[18] = {a,b,e,
                            b,c,e,
                            c,d,e,
                            d,a,e,
                            d,c,b,
                            d,b,a};

    // Cube
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE , sizeof(Vertex), (void*) (3 * sizeof(float)));

    // Pyramid
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE , sizeof(Vertex), (void*) (3 * sizeof(float)));
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
void MainView :: paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    // Draw here
    glBindVertexArray(vao1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //glBindVertexArray(vao2);
    //glDrawArrays(GL_TRIANGLES, 0, 18);


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
