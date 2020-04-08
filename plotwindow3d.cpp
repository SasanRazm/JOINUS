#include "plotwindow3d.h"
#include "ui_plotwindow3d.h"


PlotWindow3D::PlotWindow3D(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotWindow3D)
{
    ui->setupUi(this);
}

//void PlotWindow3D::drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity)
//{
//    auto *geometry = new Qt3DRender::QGeometry(_rootEntity);

//    // position vertices (start and end)
//    QByteArray bufferBytes;
//    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
//    float *positions = reinterpret_cast<float*>(bufferBytes.data());
//    *positions++ = start.x();
//    *positions++ = start.y();
//    *positions++ = start.z();
//    *positions++ = end.x();
//    *positions++ = end.y();
//    *positions++ = end.z();

//    auto *buf = new Qt3DRender::QBuffer(geometry);
//    auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
//    //auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
//    //auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, geometry);

//    buf->setData(bufferBytes);

//    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
//    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
//    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
//    positionAttribute->setVertexSize(3);
//    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
//    positionAttribute->setBuffer(buf);
//    positionAttribute->setByteStride(3 * sizeof(float));
//    positionAttribute->setCount(2);
//    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

//    // connectivity between vertices
//    QByteArray indexBytes;
//    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
//    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
//    *indices++ = 0;
//    *indices++ = 1;

//    indexBuffer->setData(indexBytes);

//    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
//    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
//    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
//    indexAttribute->setBuffer(indexBuffer);
//    indexAttribute->setCount(2);
//    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

//    // mesh
//    auto *line = new Qt3DRender::QGeometryRenderer(_rootEntity);
//    line->setGeometry(geometry);
//    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
//    auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
//    material->setAmbient(color);

//    // entity
//    auto *lineEntity = new Qt3DCore::QEntity(_rootEntity);
//    lineEntity->addComponent(line);
//    lineEntity->addComponent(material);
//}


//void PlotWindow3D::drawTriangles(const QPolygonF polygon, QColor color, Qt3DCore::QEntity *_rootEntity)
//{
//    int numOfVertices = polygon.size();
//    auto *geometry = new Qt3DRender::QGeometry(_rootEntity);
//    float dFittedMaxMapY_,dFittedMaxMapX_,dFittedMinMapY_,dFittedMinMapX_;

//    // Create and fill vertex buffer
//    QByteArray bufferBytes;
//    bufferBytes.resize(3 * numOfVertices * static_cast<int>(sizeof(float)));
//    float *positions = reinterpret_cast<float*>(bufferBytes.data());

//    for(auto point : polygon){
//        float fHalfMapWidth = (dFittedMaxMapX_ - dFittedMinMapX_) / 2;
//        float fHalfMapHeight = (dFittedMaxMapY_ - dFittedMinMapY_) / 2;

//        *positions++ = static_cast<float>(point.x() - (dFittedMinMapX_ + fHalfMapWidth));
//        *positions++ = 0.0f; //We need to drow only on the surface
//        *positions++ = static_cast<float>(point.y() - (dFittedMinMapY_ + fHalfMapHeight));
//    }

//    auto *buf = new Qt3DRender::QBuffer(geometry);
//    buf->setData(bufferBytes);

//    auto *positionAttribute = new Qt3DRender::QAttribute(geometry); // Create a vertex attribute
//    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
//    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float); // In vertex buffer we store floats
//    positionAttribute->setVertexSize(3); // Size of a vertex
//    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute); // Type of an attribute
//    positionAttribute->setBuffer(buf); // Set vertex buffer
//    positionAttribute->setByteStride(3 * sizeof(float)); // Stride between vertices
//    geometry->addAttribute(positionAttribute); // Add the attribute in our  Qt3DRender::QGeometry

//    // Create and fill an index buffer
//    QByteArray indexBytes;
//    indexBytes.resize(numOfVertices * static_cast<int>(sizeof(unsigned int))); // start to end
//    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
//    for(unsigned int i = 0; i < static_cast<unsigned int>(numOfVertices); ++i) {
//        *indices++ = i;
//    }

//    auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
//    indexBuffer->setData(indexBytes);

//    auto *indexAttribute = new Qt3DRender::QAttribute(geometry); // Create an index attribute
//    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt); // In the index buffer we will store Unsigned int
//    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute); // Type of an attribute
//    indexAttribute->setBuffer(indexBuffer); // Set the index buffer
//    indexAttribute->setCount(static_cast<unsigned int>(numOfVertices)); // As i understand, this is a number of vertices
//    geometry->addAttribute(indexAttribute); // Add attribute in our Qt3DRender::QGeometry


//    auto *poly = new Qt3DRender::QGeometryRenderer(_rootEntity);
//    poly->setGeometry(geometry);
//    //poly->setPrimitiveType(Qt3DRender::QGeometryRenderer::TriangleStrip); - Working variant
//    poly->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles); // - This is not working variant

//    //Create a material
//    auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
//    material->setAmbient(color);

//    auto *trianglesEntity = new Qt3DCore::QEntity(_rootEntity);
//    trianglesEntity->addComponent(poly); // Add our primitives
//    trianglesEntity->addComponent(material); // Add material
//}
PlotWindow3D::~PlotWindow3D()
{
    delete ui;
}
