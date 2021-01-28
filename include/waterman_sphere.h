#ifndef WATERMAN_SPHERE_H_
#define WATERMAN_SPHERE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <set>
#include <libqhullcpp/Qhull.h>

#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/PointCoordinates.h>

struct compare_GlmVec3I{
    bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
    {
        return !((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z));
    }
};

class WatermanSphere {
    public:
    WatermanSphere(float root);
    ~WatermanSphere();

    void poly(float root);
    void permute(glm::vec3 e);
    void negperm();
    bool validate(glm::vec3 e);

    void init();
    void upload();
    void draw();

    void uploadTriangle();
    void drawTriangle();
    
    void uploadPointCloud();
    void drawPointCloud();

    void uploadLine();
    void drawLine();

    /*Debug*/
    void checkForDuplicates();

    private:
        float m_root;

        std::vector<glm::vec3> m_points;
        std::vector<unsigned int> m_indices;
        GLuint m_vbo_line[2];        
        GLuint m_vbo_pc[2];
        GLuint m_vbo[2];
        GLuint m_vao;

        unsigned int m_indicesSize;
        unsigned int m_pointsSize;

        //Qhull
        orgQhull::Qhull qhull;
        orgQhull::PointCoordinates *m_externalPoints;
};

#endif