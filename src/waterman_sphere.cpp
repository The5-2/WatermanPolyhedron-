#include "../include/waterman_sphere.h"
#include "../include/openGlError.h"

#include <iostream>
#include <iterator>

#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullVertexSet.h>
#include <libqhullcpp/QhullRidge.h>

#define GOLD glm::vec3(212.0f / 255.0f, 175.0f / 255.0f, 55.0f / 255.0f)
#define SILVER glm::vec3(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f)
#define GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define BLUE glm::vec3(0.0f, 0.0f, 1.0f)
#define YELLOW glm::vec3(1.0f, 1.0f, 0.0f)
#define MAGENTA glm::vec3(1.0f, 0.0f, 1.0f)
#define LIGHTSTEELBLUE glm::vec3(176.0f / 255.0f, 196.0f / 255.0f, 222.0f / 255.0f)
#define PINK glm::vec3(255.0f / 255.0f, 20.0f / 255.0f, 147.0f / 255.0f)
#define DARKTAN glm::vec3(57.0f / 255.0f, 51.0f / 255.0f, 32.0f / 255.0f)
#define DUSTYROSE glm::vec3(183.0f / 255.0f, 123.0f / 255.0f, 130.0f / 255.0f)
#define BLACK glm::vec3(0.0f, 0.0f, 0.0f)


WatermanSphere::WatermanSphere(float root){
    m_root = root;
}

WatermanSphere::~WatermanSphere(){
    delete m_externalPoints;
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(2, m_vbo);
    glDeleteBuffers(2, m_vbo_pc);
}

void WatermanSphere::poly(float root){
    float rad = root * 2.0f;
    std::vector<glm::vec3> points;

    for(int x = int(glm::pow(rad, 0.5f)); x > -1; x--){

        if(glm::pow(x, 2) < rad / 3.0f){
            break;
        }

        for(int y = 0; y < x+1; y++){
            if(glm::pow(x, 2) + glm::pow(y, 2) > rad){
                break;
            }

            for(int z = 0; z < y+1; z++){
                if( (x + y + z) % 2 == 0){
                    if(glm::pow(x, 2) + glm::pow(y, 2) + glm::pow(z, 2) > rad){
                        break;
                    }

                    if(glm::pow(x, 2) + glm::pow(y, 2) + glm::pow(z, 2) == rad){
                        points.push_back(glm::vec3(x,y,z));
                    }
                }
            }
        }
    }

    if(points.size() > 0){
        for(unsigned int i = 0; i < points.size(); i++){
            permute(points[i]);
        }
    }
}

bool WatermanSphere::validate(glm::vec3 e){  
    for(unsigned int i = 0; i < m_points.size(); i++){
        if(m_points[i] == e){
            return false;
        }
    }

    return true;
}

void WatermanSphere::permute(glm::vec3 e){    
    for(unsigned int i = 0; i < 3; i++){
        for(unsigned int j = 0; j < 3; j++){
            for(unsigned int k = 0; k < 3; k++){
                if( (i != j) && (j != k) && (i != k)){
                    if(validate(glm::vec3(e[i], e[j], e[k]))){
                        m_points.emplace_back(glm::vec3(e[i], e[j], e[k]));
                    }
                }
            }
        }
    }
}

void WatermanSphere::negperm(){
    unsigned int size = m_points.size();

    for(unsigned int i = 0; i < size; i++){
        glm::vec3 v = m_points[i];
        glm::vec3 w;

        w = glm::vec3( v.x,-v.y, v.z);
        if(validate(w)){
            m_points.push_back(w);
        }

        w = glm::vec3( v.x, v.y,-v.z);
        if(validate(w)){
            m_points.push_back(w);
        }

        w = glm::vec3( v.x,-v.y,-v.z);
        if(validate(w)){
            m_points.push_back(w);
        }

        w = glm::vec3(-v.x, v.y, v.z);
        if(validate(w)){    
            m_points.push_back(w);        
        }

        w = glm::vec3(-v.x,-v.y, v.z);
        if(validate(w)){
            m_points.push_back(w);
        }

        w = glm::vec3(-v.x, v.y,-v.z);
        if(validate(w)){
            m_points.push_back(w);
        }

        w = glm::vec3(-v.x,-v.y,-v.z);
        if(validate(w)){
            m_points.push_back(w);
        }
    }
}

void WatermanSphere::init(){
    poly(m_root);

    for(unsigned int i = 1; i < 10; i++){
        if(int(m_root) > (i+1)){
            poly(m_root - float(i));
        }
    }

    negperm();

    checkForDuplicates();

    std::cout << "m_points size: " << m_points.size() << std::endl;
}

void WatermanSphere::upload(){
    for(unsigned int i = 0; i < m_points.size(); i++){
    	m_indices.push_back(i);
    }

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(2, m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_points.size() * sizeof(int) * 3, m_points.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3,  GL_INT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void WatermanSphere::draw(){
	glBindVertexArray(m_vao);
	glDrawElements( GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void WatermanSphere::checkForDuplicates(){
    bool foundDuplicate = false;
    unsigned int numDuplicates = 0;

    for (std::size_t i = 0; i < m_points.size(); ++i) {
        for (std::size_t j = i + 1; j < m_points.size(); ++j) {
            if (m_points[i] == m_points[j]){
                foundDuplicate = true;
                std::cout << "Duplicate at: " << i << "/" << j << " (" << m_points[i].x << ", " << m_points[i].y << ", " << m_points[i].z << ")" << std::endl;
                numDuplicates++;
            }
        }
    }
    
    if(!foundDuplicate){
        std::cout << "No duplicate found" << std::endl;
    }else{
        std::cout << "Number of duplicates: " << numDuplicates << std::endl;
    }
}

/***************
*Triangle (qhull)
***************/
void WatermanSphere::uploadTriangle(){
    m_externalPoints = new orgQhull::PointCoordinates(3, "Just a comment");  //3 = dimension
    std::vector<double> allPoints;
    for(unsigned int i = 0; i < m_points.size(); i++){
        allPoints.push_back(m_points[i].x);
        allPoints.push_back(m_points[i].y);
        allPoints.push_back(m_points[i].z);
    }

    m_externalPoints->append(allPoints); //convert to vector<double>

    qhull.runQhull(m_externalPoints->comment().c_str(), 
    m_externalPoints->dimension(), 
    m_externalPoints->count(), 
    &*m_externalPoints->coordinates(), 
    "Qt");  //http://www.qhull.org/html/qhull.htm

    std::vector<glm::vec3> qhullResults;
    std::vector<glm::vec3> qhullResultsNormal;

    orgQhull::QhullFacetList facets = qhull.facetList();

    for (orgQhull::QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it){
        if (!(*it).isGood()) continue;
        orgQhull::QhullFacet f = *it;
        orgQhull::QhullVertexSet vSet = f.vertices();

        glm::vec3 normal;
        if(f.hyperplane().isValid()){
            auto coord = f.hyperplane().coordinates();
            normal = glm::vec3(coord[0], coord[1], coord[2]);
            // double offset = f.hyperplane().offset(); //Plane can be described as normal and its offset from the center
        }else{
            std::cout << "Error: No hyperplane found for facet" << std::endl;
        }

        for (orgQhull::QhullVertexSet::iterator vIt = vSet.begin(); vIt != vSet.end(); ++vIt){
            orgQhull::QhullVertex v = *vIt;
            orgQhull::QhullPoint p = v.point();
            double * coords = p.coordinates();
            
            glm::vec3 aPoint = glm::vec3(coords[0], coords[1], coords[2]);
            qhullResults.push_back(aPoint);
            qhullResultsNormal.push_back(normal);
        }
    }

    m_pointsSize = qhullResults.size();

    /*Bind Vertices*/
	glGenBuffers(2, m_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,         
        qhullResults.size() * sizeof(float) * 3, 
        &qhullResults[0].x, 
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,         
        qhullResultsNormal.size() * sizeof(float) * 3, 
        &qhullResultsNormal[0].x, 
        GL_STATIC_DRAW);
}

void WatermanSphere::drawTriangle(){
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, m_pointsSize);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/***************
*Pointcloud
***************/
void WatermanSphere::uploadPointCloud(){
    /*Find colors*/
    std::vector<glm::vec3> color;
    color.reserve(m_points.size());

    for(unsigned int i = 0; i < m_points.size(); i++){
        float dotV = glm::dot(m_points[i], m_points[i]);
        
        if (dotV == 2.0 *  m_root){
            color.emplace_back(GOLD);
        }else if (dotV == 2.0 *  (m_root - 1.0f)){
            color.emplace_back(SILVER);
        }else if (dotV == 2.0 *  (m_root - 2.0f)){
            color.emplace_back(GREEN);
        }else if (dotV == 2.0 *  (m_root - 3.0f)){
            color.emplace_back(BLUE);
        }else if (dotV == 2.0 *  (m_root - 4.0f)){
            color.emplace_back(YELLOW);
        }else if (dotV == 2.0 *  (m_root - 5.0f)){
            color.emplace_back(MAGENTA);
        }else if (dotV == 2.0 *  (m_root - 6.0f)){
            color.emplace_back(LIGHTSTEELBLUE);
        }else if (dotV == 2.0 *  (m_root - 7.0f)){
            color.emplace_back(PINK);
        }else if (dotV == 2.0 *  (m_root - 8.0f)){
            color.emplace_back(DARKTAN);
        }else if (dotV == 2.0 *  (m_root - 9.0f)){
            color.emplace_back(DUSTYROSE);
        }else{
            color.emplace_back(BLACK);
        }
    }

    /*Bind position*/
    glGenBuffers(2, m_vbo_pc);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pc[0]);
    glBufferData(GL_ARRAY_BUFFER,         
        m_points.size() * sizeof(float) * 3, 
        &m_points[0].x, 
        GL_STATIC_DRAW);

    /*Bind color*/
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pc[1]);
    glBufferData(GL_ARRAY_BUFFER,         
        color.size() * sizeof(float) * 3, 
        &color[0].x, 
        GL_STATIC_DRAW);
}

void WatermanSphere::drawPointCloud(){
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pc[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pc[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_POINTS, 0, m_points.size());

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/***************
*Line (qhull)
***************/
void WatermanSphere::uploadLine(){
    m_externalPoints = new orgQhull::PointCoordinates(3, "Just a comment");  //3 = dimension
    std::vector<double> allPoints;
    for(unsigned int i = 0; i < m_points.size(); i++){
        allPoints.push_back(m_points[i].x);
        allPoints.push_back(m_points[i].y);
        allPoints.push_back(m_points[i].z);
    }

    m_externalPoints->append(allPoints); //convert to vector<double>

    qhull.runQhull(m_externalPoints->comment().c_str(), 
    m_externalPoints->dimension(), 
    m_externalPoints->count(), 
    &*m_externalPoints->coordinates(), 
    "o");  //http://www.qhull.org/html/qhull.htm

    std::vector<glm::vec3> qhullResults;
    std::vector<unsigned int> qhullIndices;

    orgQhull::QhullFacetList facets = qhull.facetList();

    unsigned int counter = 0;

    for (orgQhull::QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it){

        if (!(*it).isGood()) continue;
        orgQhull::QhullFacet f = *it;
        orgQhull::QhullRidgeSet rSet = f.ridges();

        for (orgQhull::QhullRidgeSet::iterator rIt = rSet.begin(); rIt != rSet.end(); ++rIt){
            orgQhull::QhullRidge r = *rIt;
            orgQhull::QhullVertexSet vSet = r.vertices();

            for (orgQhull::QhullVertexSet::iterator vIt = vSet.begin(); vIt != vSet.end(); ++vIt){
                orgQhull::QhullVertex v = *vIt;
                orgQhull::QhullPoint p = v.point();
                double * coords = p.coordinates();
                glm::vec3 aPoint = glm::vec3(coords[0], coords[1], coords[2]);
                qhullResults.push_back(aPoint);
                qhullIndices.push_back(counter);
                counter++;
            }

        }
    }

    /*Bind Vertices*/
    glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(2, m_vbo);

    /*Bind Vertices*/
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 
        qhullResults.size() * sizeof(float) * 3, 
        &qhullResults[0].x, 
        GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3,  GL_FLOAT, GL_FALSE, 0, 0);

    /*Bind Indices*/
    m_indicesSize = qhullIndices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        qhullIndices.size() * sizeof(unsigned int), 
        qhullIndices.data(), 
        GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void WatermanSphere::drawLine(){
	glBindVertexArray(m_vao);
	glDrawElements(GL_LINES, m_indicesSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}