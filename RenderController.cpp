/////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  Render Controller
//  -----------------------------
//  
//  We're using the Model-View-Controller pattern
//  so most of the control logic goes here
//  which means we need a slot for substantially
//  every possible UI manipulation
//
/////////////////////////////////////////////////////////////////

#include "RenderController.h"
#include <stdio.h>

// constructor
RenderController::RenderController
        (
        // the geometric object to show
        DirectedEdgeSurface      *newDirectedEdgeSurface,
        // the render parameters to use
        RenderParameters    *newRenderParameters,
        // the render window that it controls
        RenderWindow        *newRenderWindow
        )
    :
    directedEdgeSurface  (newDirectedEdgeSurface),
    renderParameters(newRenderParameters),
    renderWindow    (newRenderWindow),
    dragButton      (Qt::NoButton),
    startx(-1), starty(-1),
    vertexID(-1)
    { // RenderController::RenderController()
    
    // connect up signals to slots

    // signals for arcballs
    QObject::connect(   renderWindow->modelRotator,                 SIGNAL(RotationChanged()),
                        this,                                       SLOT(objectRotationChanged()));
    QObject::connect(   renderWindow->lightRotator,                 SIGNAL(RotationChanged()),
                        this,                                       SLOT(lightRotationChanged()));

    // signals for main widget to control arcball
    QObject::connect(   renderWindow->renderWidget,                 SIGNAL(MouseDown(int, int, int)),
                        this,                                       SLOT(MouseDown(int, int, int)));
    QObject::connect(   renderWindow->renderWidget,                 SIGNAL(MouseDrag(int, int)),
                        this,                                       SLOT(MouseDrag(int, int)));
    QObject::connect(   renderWindow->renderWidget,                 SIGNAL(MouseUp(int, int)),
                        this,                                       SLOT(MouseUp(int, int)));

    // signal for zoom slider
    QObject::connect(   renderWindow->zoomSlider,                   SIGNAL(valueChanged(int)),
                        this,                                       SLOT(zoomChanged(int)));

    // signal for x translate sliders
    QObject::connect(   renderWindow->xTranslateSlider,             SIGNAL(valueChanged(int)),
                        this,                                       SLOT(xTranslateChanged(int)));

    // signal for y translate slider
    QObject::connect(   renderWindow->yTranslateSlider,             SIGNAL(valueChanged(int)),
                        this,                                       SLOT(yTranslateChanged(int)));

    // signal for vertex scale slider
    QObject::connect(   renderWindow->vertexSizeSlider,	            SIGNAL(valueChanged(int)),
                        this,                                       SLOT(vertexSizeChanged(int)));

    // signal for check box for flat normals
    QObject::connect(   renderWindow->flatNormalsBox,          		SIGNAL(stateChanged(int)),
                        this,                                       SLOT(flatNormalsCheckChanged(int)));

    // signal for check box for showing vertices
    QObject::connect(   renderWindow->showVerticesBox, 		        SIGNAL(stateChanged(int)),
                        this,                                       SLOT(showVerticesCheckChanged(int)));

    // subdivision
    QObject::connect(   renderWindow->subdivideButton,              SIGNAL(clicked()),
                        this,                                       SLOT(subdivideClicked()));

    // copy the rotation matrix from the widgets to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->RotationMatrix();
    renderParameters->lightMatrix = renderWindow->lightRotator->RotationMatrix();
    } // RenderController::RenderController()

// slot for responding to arcball rotation for object
void RenderController::objectRotationChanged()
    { // RenderController::objectRotationChanged()
    // copy the rotation matrix from the widget to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->RotationMatrix();
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::objectRotationChanged()

// slot for responding to arcball rotation for light
void RenderController::lightRotationChanged()
    { // RenderController::lightRotationChanged()
    // copy the rotation matrix from the widget to the model
    renderParameters->lightMatrix = renderWindow->lightRotator->RotationMatrix();
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::lightRotationChanged()

// slot for responding to zoom slider
void RenderController::zoomChanged(int value)
    { // RenderController::zoomChanged()
    // compute the new scale
    float newZoomScale = pow(10.0, (float) value / 100.0);

    // clamp it
    if (newZoomScale < ZOOM_SCALE_MIN)
        newZoomScale = ZOOM_SCALE_MIN;
    else if (newZoomScale > ZOOM_SCALE_MAX)
        newZoomScale = ZOOM_SCALE_MAX;

    // and reset the value  
    renderParameters->zoomScale = newZoomScale;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::zoomChanged()

// slot for responding to x translate sliders
void RenderController::xTranslateChanged(int value)
    { // RenderController::xTranslateChanged()
    // reset the model's x translation (slider ticks are 1/100 each)
    renderParameters->xTranslate = (float) value / 100.0;

    // clamp it
    if (renderParameters->xTranslate < TRANSLATE_MIN)
        renderParameters->xTranslate = TRANSLATE_MIN;
    else if (renderParameters->xTranslate > TRANSLATE_MAX)
        renderParameters->xTranslate = TRANSLATE_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::xTranslateChanged()

// slot for responding to y translate slider
void RenderController::yTranslateChanged(int value)
    { // RenderController::tTranslateChanged()
    // reset the model's y translation (slider ticks are 1/100 each)
    renderParameters->yTranslate =  (float) value / 100.0;

    // clamp it
    if (renderParameters->yTranslate < TRANSLATE_MIN)
        renderParameters->yTranslate = TRANSLATE_MIN;
    else if (renderParameters->yTranslate > TRANSLATE_MAX)
        renderParameters->yTranslate = TRANSLATE_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::yTranslateChanged()

// slot for responding to vertex size slider
void RenderController::vertexSizeChanged(int value)
    { // RenderController::vertexSizeChanged()
    // reset the model's y translation (slider ticks are 1/512 each)
    renderParameters->vertexSize = (float) value / 512;

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::vertexSizeChanged()
    
// slot for toggling whether vertices are shown
void RenderController::showVerticesCheckChanged(int state)
    { // RenderController::showVerticesCheckChanged()
    // reset the model's flag
    renderParameters->showVertices = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::showVerticesCheckChanged()
     
// slot for toggling flat normals
void RenderController::flatNormalsCheckChanged(int state)
    { // RenderController::flatNormalsCheckChanged()
    // reset the model's flag
    renderParameters->useFlatNormals = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::flatNormalsCheckChanged()
     
// slots for responding to mouse manipulation
// these are general purpose signals which pass the mouse moves to the controller
// after scaling to the notional unit sphere
void RenderController::MouseDown(int whichButton, int x, int y)
    { // RenderController::MouseDown()
    } // RenderController::MouseDown()
    
// note that Continue & End assume the button has already been set
void RenderController::MouseDrag(int x, int y)
    { // RenderController::MouseDrag()
    } // RenderController::MouseDrag()

void RenderController::MouseUp(int x, int y)
    { // RenderController::MouseUp()
    } // RenderController::MouseUp()

// Perform subdivision
void RenderController::subdivideClicked()
{
    // The plan is to take the number of vertices at each iteration, and consider all vertices with an index below this to be original vertices.

    // Firstly, split all edges
    // 1. Loop through all edges somehow
    // 2. Insert a new vertex halfway across the edge. New vertices are to be added to the end of vertices.
    std::vector<int> divided;
    divided.resize(directedEdgeSurface->otherHalf.size(), -1);

    /*for (size_t halfEdgeIndex = 0; halfEdgeIndex < directedEdgeSurface->otherHalf.size(); halfEdgeIndex++)
    {
        if (!visited[halfEdgeIndex])
        {
            visited[directedEdgeSurface->otherHalf[halfEdgeIndex]] = true;
            visited[halfEdgeIndex] = true;

            std::cout << "Edge between " 
                << directedEdgeSurface->faceVertices[halfEdgeIndex] 
                << " and " << directedEdgeSurface->faceVertices[directedEdgeSurface->otherHalf[halfEdgeIndex]]
                << std::endl;

            // Split the edge
            // Create a new vertex
            Cartesian3 newVertex = directedEdgeSurface->vertices[directedEdgeSurface->faceVertices[halfEdgeIndex]] * 0.5
                                    + directedEdgeSurface->vertices[directedEdgeSurface->faceVertices[directedEdgeSurface->otherHalf[halfEdgeIndex]]] * 0.5;

            directedEdgeSurface->vertices.push_back(newVertex);

            // Update this edge and its other half to point to the new vertex
            // Other half first
            unsigned int originalOtherHalfIndex = directedEdgeSurface->otherHalf[halfEdgeIndex];
            directedEdgeSurface->otherHalf[originalOtherHalfIndex] = directedEdgeSurface->vertices.size() - 1;
            // Now this
            directedEdgeSurface->otherHalf[halfEdgeIndex] = directedEdgeSurface->vertices.size() - 1;

            // Other halves from the new vertex


            // Create two new half edges to point away from the new vertex

            // Add fde's for the new vertices
        }
    }*/

    unsigned int startVerticesLength = directedEdgeSurface->vertices.size();
    size_t startFaceLength = directedEdgeSurface->faceVertices.size();

    // Loop through all the faces. Subdivide all at once.
    for (size_t faceIndex = 0; faceIndex < startFaceLength; faceIndex+=3)
    {
        // Copy the current face vertex indices
        unsigned int startVertAIndex = directedEdgeSurface->faceVertices[faceIndex];
        unsigned int startVertBIndex = directedEdgeSurface->faceVertices[faceIndex + 1];
        unsigned int startVertCIndex = directedEdgeSurface->faceVertices[faceIndex + 2];
        
        // Create 3 new vertices and corresponding normals
        Cartesian3 vertAB = directedEdgeSurface->vertices[startVertAIndex] * 0.5 
                            + directedEdgeSurface->vertices[startVertBIndex] * 0.5;
        Cartesian3 normAB = directedEdgeSurface->normals[startVertAIndex] * 0.5
                            + directedEdgeSurface->normals[startVertBIndex] * 0.5;

        Cartesian3 vertBC = directedEdgeSurface->vertices[startVertBIndex] * 0.5
                            + directedEdgeSurface->vertices[startVertCIndex] * 0.5;
        Cartesian3 normBC = directedEdgeSurface->normals[startVertBIndex] * 0.5
                            + directedEdgeSurface->normals[startVertCIndex] * 0.5;

        Cartesian3 vertAC = directedEdgeSurface->vertices[startVertAIndex] * 0.5 
                            + directedEdgeSurface->vertices[startVertCIndex] * 0.5;
        Cartesian3 normAC = directedEdgeSurface->normals[startVertAIndex] * 0.5
                            + directedEdgeSurface->normals[startVertCIndex] * 0.5;

        // Add the vertices, or get indices of already placed
        unsigned int    vertABIndex = divided[faceIndex],
                        vertBCIndex = divided[faceIndex + 1],
                        vertACIndex = divided[faceIndex + 2];
        
        if (vertABIndex == -1)
        {
            directedEdgeSurface->vertices.push_back(vertAB);
            directedEdgeSurface->normals.push_back(normAB);
            vertABIndex = directedEdgeSurface->vertices.size() - 1;
        }
        if (vertBCIndex == -1)
        {
            directedEdgeSurface->vertices.push_back(vertBC);
            directedEdgeSurface->normals.push_back(normBC);
            vertBCIndex = directedEdgeSurface->vertices.size() - 1;
        }
        if (vertACIndex == -1)
        {
            directedEdgeSurface->vertices.push_back(vertAC);
            directedEdgeSurface->normals.push_back(normAC);
            vertACIndex = directedEdgeSurface->vertices.size() - 1;
        }

        // Flag that these were divided, with the indices of the new vertices
        divided[faceIndex] = vertABIndex;
        divided[faceIndex + 1] = vertBCIndex;
        divided[faceIndex + 2] = vertACIndex;

        // Change the face vertex indices to the new vertices
        directedEdgeSurface->faceVertices[faceIndex + 0] = vertABIndex;    // Subtraction from 1 to 3 to avoud going beyond the array
        directedEdgeSurface->faceVertices[faceIndex + 1] = vertBCIndex;
        directedEdgeSurface->faceVertices[faceIndex + 2] = vertACIndex;

        // Create 3 new faces at the end of the face array
        // First face, from original vertex A
        directedEdgeSurface->faceVertices.push_back(startVertAIndex);
        directedEdgeSurface->faceVertices.push_back(vertABIndex);   // Vertex on edge AB
        directedEdgeSurface->faceVertices.push_back(vertACIndex);   // Vertex on edge AC
        // Second face, from original vertex B
        directedEdgeSurface->faceVertices.push_back(startVertBIndex);
        directedEdgeSurface->faceVertices.push_back(vertBCIndex);   // Vertex on edge BC
        directedEdgeSurface->faceVertices.push_back(vertABIndex);   // Vertex on edge AB
        // Third face, from original vertex C
        directedEdgeSurface->faceVertices.push_back(startVertCIndex);
        directedEdgeSurface->faceVertices.push_back(vertACIndex);   // Vertex on edge AC
        directedEdgeSurface->faceVertices.push_back(vertBCIndex);   // Vertex on edge BC

        // Create half edges for the new faces
        // 3 per face- expand by 9 as 3 halfedges will be updated
        directedEdgeSurface->otherHalf.resize(directedEdgeSurface->otherHalf.size() + 9, -1);

        // Update existing half edges to be on the central face

        // Create new half edges to represent the split edge

        // Move the existing half edges 
    }
}
