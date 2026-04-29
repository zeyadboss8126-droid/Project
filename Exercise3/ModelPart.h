#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QColor>

#include <vtkSmartPointer.h>

// Forward declarations — keep VTK OpenGL headers out of this header
class vtkActor;
class vtkSTLReader;
class vtkPolyDataMapper;
class vtkShrinkPolyData;
class vtkClipPolyData;
class vtkPlane;

class ModelPart {
public:
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);
    ~ModelPart();

    void appendChild(ModelPart* item);
    ModelPart* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void set(int column, const QVariant& value);
    ModelPart* parentItem();
    int row() const;

    void setColour(const unsigned char R, const unsigned char G, const unsigned char B);
    unsigned char getColourR();
    unsigned char getColourG();
    unsigned char getColourB();

    void setVisible(bool isVisible);
    bool visible();

    void loadSTL(QString fileName);
    vtkSmartPointer<vtkActor> getActor();

    QString getName() const;
    QColor getColour() const;
    bool getVisible() const;
    void setName(const QString& name);
    void setColour(int r, int g, int b);

    // Filters
    void setShrinkFilter(bool enabled);
    void setClipFilter(bool enabled);
    bool shrinkFilterEnabled() const;
    bool clipFilterEnabled() const;

private:
    QList<ModelPart*> m_childItems;
    QList<QVariant>   m_itemData;
    ModelPart* m_parentItem;

    vtkSmartPointer<vtkSTLReader>       file;
    vtkSmartPointer<vtkPolyDataMapper>  mapper;
    vtkSmartPointer<vtkActor>           actor;

    vtkSmartPointer<vtkShrinkPolyData>  shrinkFilter;
    vtkSmartPointer<vtkClipPolyData>    clipFilter;
    vtkSmartPointer<vtkPlane>           clipPlane;

    QString m_name;
    QColor  m_colour;
    bool    m_visible = true;

    bool    m_shrinkEnabled = false;
    bool    m_clipEnabled = false;

    void updatePipeline();
};

#endif