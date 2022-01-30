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
#include <fstream>

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

    // writeout
    QObject::connect(   renderWindow->writeButton,                  SIGNAL(clicked()),
                        this,                                       SLOT(writeClicked()));

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

    size_t startFaceLength = directedEdgeSurface->faceVertices.size();

    std::vector<Cartesian3> newPositions;
    std::vector<Cartesian3> newNormals;
    newPositions.reserve(directedEdgeSurface->vertices.size());

    // Calculate new positions for old vertices, but do not apply yet
    // Count the degree of each vertex
    std::vector<int> degrees(directedEdgeSurface->vertices.size(), 0);
    for (int index : directedEdgeSurface->faceVertices)
    {
        degrees[index]++;
    }

    // Calculate new positions
    for (size_t vertIndex = 0; vertIndex < directedEdgeSurface->vertices.size(); vertIndex++)
    {
        Cartesian3 neighbourVertSum;
        Cartesian3 neighbourNormSum;
        // Get the FDE of each vertex, and traverse around it: edge1 -> edge1otherhalf -> next -> nextotherhalf..... until at start
        int startEdge = directedEdgeSurface->firstDirectedEdge[vertIndex];
        int edge = startEdge;
        do
        {
            neighbourVertSum = neighbourVertSum + directedEdgeSurface->vertices[directedEdgeSurface->faceVertices[edge]];
            neighbourNormSum = neighbourNormSum + directedEdgeSurface->normals[directedEdgeSurface->faceVertices[edge]];
            edge = ((directedEdgeSurface->otherHalf[edge] - directedEdgeSurface->otherHalf[edge] % 3) + (directedEdgeSurface->otherHalf[edge] % 3 + 1) % 3);
        } while (edge != startEdge);

        float u;
        if (degrees[vertIndex] == 3)
        {
            u = 3.0 / 16.0;
        }
        else
        {
            u = 3.0 / (8.0 * (float)degrees[vertIndex]);
        }
        Cartesian3 newPos = (1.0 - (float)degrees[vertIndex] * u) * directedEdgeSurface->vertices[vertIndex] + u * neighbourVertSum;
        Cartesian3 newNorm = (1.0 - (float)degrees[vertIndex] * u) * directedEdgeSurface->normals[vertIndex] + u * neighbourVertSum;
        newPositions.push_back(newPos);
    }

    std::vector<unsigned int> newIndices;
    newIndices.resize(directedEdgeSurface->faceVertices.size());

    // Loop through all the faces. Subdivide all at once.
    for (size_t faceIndex = 0; faceIndex < startFaceLength; faceIndex+=3)
    {
        // Copy the current face vertex indices
        unsigned int startVertAIndex = directedEdgeSurface->faceVertices[faceIndex + 0];
        unsigned int startVertBIndex = directedEdgeSurface->faceVertices[faceIndex + 1];
        unsigned int startVertCIndex = directedEdgeSurface->faceVertices[faceIndex + 2];
        
        // Create 3 new vertices and corresponding normals
        /*Cartesian3 vertAB = (directedEdgeSurface->vertices[startVertAIndex] 
                            + directedEdgeSurface->vertices[startVertBIndex]) * 3.0 / 8.0
                            + (directedEdgeSurface->vertices[startVertCIndex]
                            + directedEdgeSurface->vertices[directedEdgeSurface->faceVertices[directedEdgeSurface->otherHalf[faceIndex + 1] + 1]]) * 1.0 / 8.0;*/


        Cartesian3 vertAC = directedEdgeSurface->vertices[startVertAIndex] * 0.5
                            + directedEdgeSurface->vertices[startVertCIndex] * 0.5;
        Cartesian3 normAC = directedEdgeSurface->normals[startVertAIndex] * 0.5
                            + directedEdgeSurface->normals[startVertCIndex] * 0.5;

        /*Cartesian3 vertBC = (directedEdgeSurface->vertices[startVertBIndex] 
                            + directedEdgeSurface->vertices[startVertCIndex]) * 3.0 / 8.0
                            + (directedEdgeSurface->vertices[startVertAIndex]
                            + directedEdgeSurface->vertices[directedEdgeSurface->faceVertices[directedEdgeSurface->otherHalf[faceIndex + 1] + 1]]) * 1.0 / 8.0;*/


        Cartesian3 vertBA = directedEdgeSurface->vertices[startVertBIndex] * 0.5
                            + directedEdgeSurface->vertices[startVertAIndex] * 0.5;
        Cartesian3 normBA = directedEdgeSurface->normals[startVertBIndex] * 0.5
                            + directedEdgeSurface->normals[startVertAIndex] * 0.5;

        /*Cartesian3 vertAC = (directedEdgeSurface->vertices[startVertAIndex] 
                            + directedEdgeSurface->vertices[startVertCIndex]) * 3.0 / 8.0
                            + (directedEdgeSurface->vertices[startVertBIndex]
                            + directedEdgeSurface->vertices[directedEdgeSurface->faceVertices[directedEdgeSurface->otherHalf[faceIndex + 1] + 1]]) * 1.0 / 8.0;*/


        Cartesian3 vertCB = directedEdgeSurface->vertices[startVertCIndex] * 0.5
                            + directedEdgeSurface->vertices[startVertBIndex] * 0.5;
        Cartesian3 normCB = directedEdgeSurface->normals[startVertCIndex] * 0.5
                            + directedEdgeSurface->normals[startVertBIndex] * 0.5;

        // Add the vertices, or get indices of already placed
        int vertACIndex, vertBAIndex, vertCBIndex;

        vertACIndex = divided[faceIndex + 0];
        vertBAIndex = divided[faceIndex + 1];
        vertCBIndex = divided[faceIndex + 2];

        if (vertACIndex == -1)
        {
            directedEdgeSurface->vertices.push_back(vertAC);
            directedEdgeSurface->normals.push_back(normAC);
            vertACIndex = directedEdgeSurface->vertices.size() - 1;
        }
        if (vertBAIndex == -1)
        {
            directedEdgeSurface->vertices.push_back(vertBA);
            directedEdgeSurface->normals.push_back(normBA);
            vertBAIndex = directedEdgeSurface->vertices.size() - 1;
        }
        if (vertCBIndex == -1)
        {
            directedEdgeSurface->vertices.push_back(vertCB);
            directedEdgeSurface->normals.push_back(normCB);
            vertCBIndex = directedEdgeSurface->vertices.size() - 1;
        }

        // Mark this and other half
        divided[faceIndex + 0] = vertACIndex;
        divided[faceIndex + 1] = vertBAIndex;
        divided[faceIndex + 2] = vertCBIndex;


        divided[directedEdgeSurface->otherHalf[faceIndex + 0]] = vertACIndex;
        divided[directedEdgeSurface->otherHalf[faceIndex + 1]] = vertBAIndex;
        divided[directedEdgeSurface->otherHalf[faceIndex + 2]] = vertCBIndex;

        // Change the face vertex indices to the new vertices
        newIndices[faceIndex + 0] = vertACIndex;
        newIndices[faceIndex + 1] = vertBAIndex;
        newIndices[faceIndex + 2] = vertCBIndex;

        // Create 3 new faces at the end of the face array
        // First face, from original vertex A
        directedEdgeSurface->faceVertices.push_back(startVertCIndex);
        directedEdgeSurface->faceVertices.push_back(vertACIndex);   // Vertex on edge AB
        directedEdgeSurface->faceVertices.push_back(vertCBIndex);   // Vertex on edge AC
        // Second face, from original vertex B
        directedEdgeSurface->faceVertices.push_back(startVertAIndex);
        directedEdgeSurface->faceVertices.push_back(vertBAIndex);   // Vertex on edge BC
        directedEdgeSurface->faceVertices.push_back(vertACIndex);   // Vertex on edge AB
        // Third face, from original vertex C
        directedEdgeSurface->faceVertices.push_back(startVertBIndex);
        directedEdgeSurface->faceVertices.push_back(vertCBIndex);   // Vertex on edge AC
        directedEdgeSurface->faceVertices.push_back(vertBAIndex);   // Vertex on edge BC

        // Create half edges for the new faces
        // 3 per face- expand by 9 as 3 halfedges will be updated
        directedEdgeSurface->otherHalf.resize(directedEdgeSurface->otherHalf.size() + 9, 0);
    }

    // Apply changes to old
    for (size_t i = 0; i < newPositions.size(); i++)
    {
        directedEdgeSurface->vertices[i] = newPositions[i];
    }

    for (size_t i = 0; i < newIndices.size(); i++)
    {
        directedEdgeSurface->faceVertices[i] = newIndices[i];
    }

    // Apply changes to new
    
    // Calculating first directed edges
    directedEdgeSurface->firstDirectedEdge.resize(directedEdgeSurface->vertices.size());
    // Find each vertex's first occurance
    for (unsigned int vertexIndex = 0; vertexIndex < directedEdgeSurface->vertices.size(); vertexIndex++)
    {
        std::vector<unsigned int>::iterator it = std::find(directedEdgeSurface->faceVertices.begin(), directedEdgeSurface->faceVertices.end(), vertexIndex);
        int index = it - directedEdgeSurface->faceVertices.begin();
        if (it == directedEdgeSurface->faceVertices.end())
        {
            // This vertex is not used
            std::cout << "ERROR: Mesh contains a stranded vertex: " << vertexIndex << " " << directedEdgeSurface->vertices[vertexIndex] << " (it is not used on any faces)" << std::endl;
        }
        // Use the next index on face for first directed edge
        // Ensures the first directed edge is pointing to another vertex on this face
        int thisFirstDirectedEdge = (index - index % 3) + (index % 3 + 1) % 3;
        directedEdgeSurface->firstDirectedEdge[vertexIndex] = thisFirstDirectedEdge;
    }

    // Clear existing other halves
    std::fill(directedEdgeSurface->otherHalf.begin(), directedEdgeSurface->otherHalf.end(), -1);

    // Calculating other halves
    for (unsigned int firstVertexIndex = 0; firstVertexIndex < directedEdgeSurface->faceVertices.size(); firstVertexIndex++)
    {
        // Skip this edge if it's already been modified
        if (directedEdgeSurface->otherHalf[firstVertexIndex] >= 0)
        {
            continue;
        }

        // Get the index of the vertex pointing to this one
        int firstVertexPreviousIndex = (firstVertexIndex - (firstVertexIndex % 3)) + (firstVertexIndex % 3 + 2) % 3;
        int firstVertex = directedEdgeSurface->faceVertices[firstVertexIndex];
        int firstVertexPrevious = directedEdgeSurface->faceVertices[firstVertexPreviousIndex];
        // Find opposite- i.e. for edge b -> a, find edge a -> b
        // Search the remainder of the faces- if a pair lies in the past, an edge is shared by 3 faces (non-manifold, and cannot be stored in this structure anyway)
        for (unsigned int faceIndex = (firstVertexIndex / 3) + 1; faceIndex < directedEdgeSurface->faceVertices.size() / 3; faceIndex++)
        {
            // Check edge combos for this face
            for (unsigned int i = 0; i < 3; i++)
            {
                int secondVertexIndex = faceIndex * 3 + i;
                int secondVertex = directedEdgeSurface->faceVertices[secondVertexIndex];
                int secondVertexNextIndex = faceIndex * 3 + ((i + 1) % 3);
                int secondVertexNext = directedEdgeSurface->faceVertices[secondVertexNextIndex];

                // Check if this is the opposite edge
                if (secondVertex == firstVertex && secondVertexNext == firstVertexPrevious)
                {
                    // Check this edge isn't already paired- may occur if more than 2 faces to an edge
                    if (directedEdgeSurface->otherHalf[firstVertexIndex] >= 0)
                    {
                        std::cout << "ERROR: Two copies of edge from " << firstVertexPrevious << " to " << firstVertex << ". Copy is on face " << (firstVertexIndex / 3) << std::endl;
                    }
                    // Set BOTH other halves
                    directedEdgeSurface->otherHalf[firstVertexIndex] = secondVertexNextIndex;
                    directedEdgeSurface->otherHalf[secondVertexNextIndex] = firstVertexIndex;
                }
            }
        }
        // If the index is still -1, a paired edge could not be found
        if (directedEdgeSurface->otherHalf[firstVertexIndex] == -1)
        {
            std::cout << "ERROR: A paired edge could not be found for edge from " << firstVertexPrevious << " to " << firstVertex << " on face " << (firstVertexIndex / 3) << std::endl;
        }
    }

    // Update the interface
    renderWindow->ResetInterface();
}

void RenderController::writeClicked()
{
    // Spawn file browser to get name
    QString fileName = QFileDialog::getSaveFileName(renderWindow, tr("Save .diredgenormal File"), "./", tr("diredgenormal files (*.diredgenormal)"));
    if (!fileName.isEmpty())
    {
        // Init outstream
        std::ofstream ofs(fileName.toStdString());
        if (ofs.good())
        {
            directedEdgeSurface->WriteObjectStream(ofs);
            QMessageBox successMsgBox;
            successMsgBox.setText("diredgenormal file saved");
            successMsgBox.exec();
        }
        else
        {
            QMessageBox failMsgBox;
            failMsgBox.setText("Could not save file");
            failMsgBox.exec();
        }
    }
}
