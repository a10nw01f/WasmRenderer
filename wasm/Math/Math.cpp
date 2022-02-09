#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp> 

glm::mat4 CreateTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
{
    return  glm::translate(glm::mat4(1.f), pos) * 
            glm::scale(glm::mat4(1.f), scale) * 
            glm::eulerAngleYXZ(rot.y, rot.x, rot.z);      
}