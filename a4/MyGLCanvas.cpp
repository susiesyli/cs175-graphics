#define NUM_OPENGL_LIGHTS 8

#include "MyGLCanvas.h"
#include "math.h"

int Shape::m_segmentsX;
int Shape::m_segmentsY;
const float INTERSECTION_EPSILON = 1e-3f; 
const float SHADOW_EPSILON = INTERSECTION_EPSILON * 2; 

// struct for storing each object in the scenegraph described by an xml file
// struct SceneObject {
// 	glm::mat4 transformMatrix;
// 	std::shared_ptr<ScenePrimitive> primitive;  // Use smart pointer
// };
std::vector<SceneObject> sceneObjects;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	eyePosition = glm::vec3(2.0f, 2.0f, 2.0f);

	pixelWidth = w;
	pixelHeight = h;

	isectOnly = 1;
	segmentsX = segmentsY = 10;
	scale = 1.0f;
	parser = NULL;

	objType = SHAPE_CUBE;
	cube = new Cube();
	cylinder = new Cylinder();
	cone = new Cone();
	sphere = new Sphere();
	shape = cube;

	shape->setSegments(segmentsX, segmentsY);

	camera = new Camera();
	camera->orientLookAt(eyePosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

MyGLCanvas::~MyGLCanvas() {
	delete cube;
	delete cylinder;
	delete cone;
	delete sphere;
	if (camera != NULL) {
		delete camera;
	}
	if (parser != NULL) {
		delete parser;
	}
	if (pixels != NULL) {
		delete pixels;
	}
}

float blendHelper(float original, float textureValue, float blend) {
	return (1 - blend) * original + (blend * textureValue);
}


void printMatrix(const glm::mat4& matrix) {
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			std::cout << matrix[row][col] << " ";
		}
		std::cout << std::endl;
	}
}

void printVector(const glm::vec3& vector) {
	std::cout << "glm::vec3(" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
}

void printVector(const glm::vec4& vector) {
	std::cout << "glm::vec4(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
}

/* The generateRay function accepts the mouse click coordinates
	(in x and y, which will be integers between 0 and screen width and 0 and screen height respectively).
   The function returns the ray
*/
glm::vec3 MyGLCanvas::generateRay(int pixelX, int pixelY) {
	int screenWidth = w();
	int screenHeight = h();

	// normalize pixels to [-1, 1]
	float normEyeX = ((float(pixelX) / float(screenWidth)) - 0.5) * 2;
	float normEyeY = ((float(pixelY) / float(screenHeight)) - 0.5) * 2;

	// Create point in normalized device coordinates (NDC)
	glm::vec4 screenSpacePoint(normEyeX, normEyeY, -1.0f, 1.0f);

	// Get combined inverse projection-view matrix
	glm::mat4 inverseProjView = glm::inverse(camera->getProjectionMatrix() * camera->getModelViewMatrix());

	// Transform to world space
	glm::vec4 worldSpacePoint = inverseProjView * screenSpacePoint;

	// Perform perspective divide
	worldSpacePoint /= worldSpacePoint.w;

	// Calculate ray direction
	glm::vec3 ray = glm::normalize(glm::vec3(worldSpacePoint) - getEyePoint());

	return ray;
}

glm::vec3 MyGLCanvas::getEyePoint() {
	return camera->getEyePoint(); // tie this to camera position
}

/* The getIsectPointWorldCoord function accepts three input parameters:
	(1) the eye point (in world coordinate)
	(2) the ray vector (in world coordinate)
	(3) the "t" value

	The function should return the intersection point on the sphere
*/
glm::vec3 MyGLCanvas::getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t) {
	glm::vec3 p = eye + t * ray;
	return p;
}


float MyGLCanvas::renderShape(OBJ_TYPE type, glm::vec3 ray, glm::mat4 transformMatrix, glm::vec3 eyePoint) {
	objType = type;
	switch (type) {
	case SHAPE_CUBE:
		shape = cube;
		break;
	case SHAPE_CYLINDER:
		shape = cylinder;
		shape = cylinder;
		break;
	case SHAPE_CONE:
		shape = cone;
		break;
	case SHAPE_SPHERE:
		shape = sphere;
		break;
	case SHAPE_SPECIAL1:
	default:
		shape = cube;
	}

	shape->setSegments(segmentsX, segmentsY);
	return shape->draw(eyePoint, ray, transformMatrix);
}


void MyGLCanvas::loadSceneFile(const char* filenamePath) {
	if (parser != nullptr) {
		delete parser;
		parser = nullptr;
	}

	sceneObjects.clear();

	parser = new SceneParser(filenamePath);

	bool success = parser->parse();
	cout << "success? " << success << endl;
	if (success == false) {
		delete parser;
		parser = NULL;
	}
	else {
		SceneCameraData cameraData;
		parser->getCameraData(cameraData);
		parser->getGlobalData(globalSceneData);
		camera->reset();
		camera->setViewAngle(cameraData.heightAngle);
		if (cameraData.isDir == true) {
			camera->orientLookVec(cameraData.pos, cameraData.look, cameraData.up);
		}
		else {
			camera->orientLookAt(cameraData.pos, cameraData.lookAt, cameraData.up);
		}
	}
}

void MyGLCanvas::traverseSceneGraph(SceneNode* node, const glm::mat4& parentTransform)
{
	if (!node) return;

	// iterate over and store transformations for each node
	glm::mat4 currentTransform = parentTransform;
	for (const auto& transform : node->transformations)
	{
		switch (transform->type)
		{
		case TRANSFORMATION_TRANSLATE:
			currentTransform = glm::translate(currentTransform, transform->translate);
			break;
		case TRANSFORMATION_SCALE:
			currentTransform = glm::scale(currentTransform, transform->scale);
			break;
		case TRANSFORMATION_ROTATE:
			currentTransform = glm::rotate(currentTransform, transform->angle, transform->rotate);
			break;
		case TRANSFORMATION_MATRIX:
			currentTransform = currentTransform * transform->matrix;
			break;
		}
	}

	// add each primitive and computed transforms to sceneObjects
	for (const auto& primitive : node->primitives) {
		sceneObjects.push_back({
			currentTransform,
			primitive // point
		});
	}

	// clear primitive pointers from node to prevent double deletion
	//node->primitives.clear();

	// recursively traverse children nodes
	for (SceneNode* child : node->children)
	{
		traverseSceneGraph(child, currentTransform);
	}
}

void MyGLCanvas::setSegments() {
	shape->setSegments(segmentsX, segmentsY);
}

void MyGLCanvas::draw() {
	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (parser == NULL) {
		return;
	}

	if (pixels == NULL) {
		return;
	}

	//this just draws the "pixels" to the screen
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(pixelWidth, pixelHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

int MyGLCanvas::handle(int e) {
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_KEYUP:
		printf("keyboard event: key pressed: %c\n", Fl::event_key());
		break;
	case FL_MOUSEWHEEL:
		break;
	}

	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	if (camera != NULL) {
		camera->setScreenSize(w, h);
	}
	puts("resize called");
}

void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	camera->setScreenSize(width, height);
}

//Given the pixel (x, y) position, set its color to (r, g, b)
void MyGLCanvas::setpixel(GLubyte* buf, int x, int y, int r, int g, int b) {
	pixelWidth = camera->getScreenWidth();
	buf[(y*pixelWidth + x) * 3 + 0] = (GLubyte)r;
	buf[(y*pixelWidth + x) * 3 + 1] = (GLubyte)g;
	buf[(y*pixelWidth + x) * 3 + 2] = (GLubyte)b;
}

float MyGLCanvas::calculateObjectLighting(SceneObject object, glm::vec3 objNormal, color color, int numLights, glm::vec3 worldSpacePos, float blend, glm::vec3 textureMap, glm::vec3 viewOrigin) {
    float intensity = 0.0f;
    SceneLightData lightData;
    
    // initialize intensity with unblended ambient term for shadowed-areas 
    switch (color) {
        case RED:
            intensity = globalSceneData.ka * object.primitive->material.cAmbient.r;
            break;
        case GREEN: 
            intensity = globalSceneData.ka * object.primitive->material.cAmbient.g;
            break;
        case BLUE:
            intensity = globalSceneData.ka * object.primitive->material.cAmbient.b;
            break;
    }

    // loop through lights for diffuse and specular calculations
    for (int i = 0; i < parser->getNumLights(); i++) {
        parser->getLightData(i, lightData);
        float li = 0.0f;
        float od = 0.0f;
        float os = 0.0f;
        
        switch (color) {
            case RED:
                li = lightData.color.r;
                od = object.primitive->material.cDiffuse.r;
                os = object.primitive->material.cSpecular.r;
                break;
            case GREEN:
                li = lightData.color.g;
                od = object.primitive->material.cDiffuse.g;
                os = object.primitive->material.cSpecular.g;
                break;
            case BLUE:
                li = lightData.color.b;
                od = object.primitive->material.cDiffuse.b;
                os = object.primitive->material.cSpecular.b;
                break;
        }

        glm::vec3 normLightVector;
        float lightDistance;
        if (lightData.type == LIGHT_POINT) { 
            normLightVector = glm::normalize(lightData.pos - worldSpacePos); 
            lightDistance = glm::length(lightData.pos - worldSpacePos);
        } else {
            normLightVector = glm::normalize(-lightData.dir);
            lightDistance = std::numeric_limits<float>::max();
        }

        // handle shadow check
        glm::vec3 shadowRayOrigin = worldSpacePos + SHADOW_EPSILON * normLightVector;
        if (!isInShadow(shadowRayOrigin, normLightVector, lightDistance)) {
            // in non-shadow areas, blend (ambient and diffuse) with texture
            float blendedValue = 0.0f;
            switch (color) {
                case RED:
                    blendedValue = blendHelper((globalSceneData.ka * object.primitive->material.cAmbient.r + globalSceneData.kd * od), textureMap.x, blend);
                    break;
                case GREEN:
                    blendedValue = blendHelper((globalSceneData.ka * object.primitive->material.cAmbient.g + globalSceneData.kd * od), textureMap.y, blend);
                    break;
                case BLUE:
                    blendedValue = blendHelper((globalSceneData.ka * object.primitive->material.cAmbient.b + globalSceneData.kd * od), textureMap.z, blend);
                    break;
            }

            // first remove the base ambient in the shadow case 
            intensity -= (color == RED ? globalSceneData.ka * object.primitive->material.cAmbient.r :
                        color == GREEN ? globalSceneData.ka * object.primitive->material.cAmbient.g :
                        globalSceneData.ka * object.primitive->material.cAmbient.b);

            float nDotL = glm::max(glm::dot(glm::normalize(objNormal), normLightVector), 0.0f);
            intensity += li * (blendedValue * nDotL);

            // specular term (unblended) - now using viewOrigin instead of camera position
            glm::vec3 viewVector = glm::normalize(viewOrigin - worldSpacePos);
            glm::vec3 reflectVector = glm::normalize(glm::reflect(-normLightVector, objNormal));
            float rDotV = glm::max(glm::dot(reflectVector, viewVector), 0.0f);
            float specularTerm = globalSceneData.ks * os * pow(rDotV, object.primitive->material.shininess);
            intensity += li * specularTerm;
        }
    }

    return glm::clamp(intensity, 0.0f, 1.0f);
}


// // GOOD!
// float MyGLCanvas::calculateObjectLighting(SceneObject object, glm::vec3 objNormal, color color, int numLights, glm::vec3 worldSpacePos, float blend, glm::vec3 textureMap) {
//     float intensity = 0.0f;
//     SceneLightData lightData;
    
//     // initialize intensity with unblended ambient term, which will be used for 
//     // shadowed-areas 
//     switch (color) {
//         case RED:
//             intensity = globalSceneData.ka * object.primitive->material.cAmbient.r;
//             break;
//         case GREEN: 
//             intensity = globalSceneData.ka * object.primitive->material.cAmbient.g;
//             break;
//         case BLUE:
//             intensity = globalSceneData.ka * object.primitive->material.cAmbient.b;
//             break;
//     }

//     // loop through lights for diffuse and specular calculations
//     for (int i = 0; i < parser->getNumLights(); i++) {
//         parser->getLightData(i, lightData);
//         float li = 0.0f;
//         float od = 0.0f;
//         float os = 0.0f;
        
//         switch (color) {
//             case RED:
//                 li = lightData.color.r;
//                 od = object.primitive->material.cDiffuse.r;
//                 os = object.primitive->material.cSpecular.r;
//                 break;
//             case GREEN:
//                 li = lightData.color.g;
//                 od = object.primitive->material.cDiffuse.g;
//                 os = object.primitive->material.cSpecular.g;
//                 break;
//             case BLUE:
//                 li = lightData.color.b;
//                 od = object.primitive->material.cDiffuse.b;
//                 os = object.primitive->material.cSpecular.b;
//                 break;
//         }

//         glm::vec3 normLightVector;
//         float lightDistance;
//         if (lightData.type == LIGHT_POINT) { 
//             normLightVector = glm::normalize(lightData.pos - worldSpacePos); 
//             lightDistance = glm::length(lightData.pos - worldSpacePos);
//         } else {
//             normLightVector = glm::normalize(-lightData.dir);
//             lightDistance = std::numeric_limits<float>::max();
//         }

//         // handle shadow check
//         glm::vec3 shadowRayOrigin = worldSpacePos + SHADOW_EPSILON * normLightVector;
//         if (!isInShadow(shadowRayOrigin, normLightVector, lightDistance)) {
//             // in non-shadow areas, blend (ambient and diffuse) with texture
//             float blendedValue = 0.0f;
//             switch (color) {
//                 case RED:
//                     blendedValue = blendHelper((globalSceneData.ka * object.primitive->material.cAmbient.r + globalSceneData.kd * od), textureMap.x, blend);
//                     break;
//                 case GREEN:
//                     blendedValue = blendHelper((globalSceneData.ka * object.primitive->material.cAmbient.g + globalSceneData.kd * od), textureMap.y, blend);
//                     break;
//                 case BLUE:
//                     blendedValue = blendHelper((globalSceneData.ka * object.primitive->material.cAmbient.b + globalSceneData.kd * od), textureMap.z, blend);
//                     break;
//             }

//             // first remove the base ambient in the shadow case 
//             intensity -= (color == RED ? globalSceneData.ka * object.primitive->material.cAmbient.r :
//                         color == GREEN ? globalSceneData.ka * object.primitive->material.cAmbient.g :
//                         globalSceneData.ka * object.primitive->material.cAmbient.b);

//             float nDotL = glm::max(glm::dot(glm::normalize(objNormal), normLightVector), 0.0f);
//             intensity += li * (blendedValue * nDotL);

//             // specular term (unblended)
//             glm::vec3 viewVector = glm::normalize(camera->getEyePoint() - worldSpacePos);
//             glm::vec3 reflectVector = glm::normalize(glm::reflect(-normLightVector, objNormal));
//             float rDotV = glm::max(glm::dot(reflectVector, viewVector), 0.0f);
//             float specularTerm = globalSceneData.ks * os * pow(rDotV, object.primitive->material.shininess);
//             intensity += li * specularTerm;
//         }
//     }

//     return glm::clamp(intensity, 0.0f, 1.0f);
// }

bool MyGLCanvas::isInShadow(const glm::vec3& point, const glm::vec3& lightDir, float lightDistance) {
	glm::vec3 shadowRayOrigin = point + SHADOW_EPSILON * lightDir; // Offset to prevent self-intersection

	// Check for intersections between point and light
	for (auto& obj : sceneObjects) {
		float t = renderShape(obj.primitive->type, lightDir, obj.transformMatrix, shadowRayOrigin);
		// If we find any intersection before the light, the point is in shadow
		if (t > SHADOW_EPSILON && t < lightDistance) {
			return true;
		}
	}
	return false;
}

void MyGLCanvas::renderScene() {
	cout << "render button clicked!" << endl;

	if (parser == NULL) {
		cout << "no scene loaded yet" << endl;
		return;
	}
	sceneObjects.clear();
	SceneNode* root = parser->getRootNode();
	glm::mat4 compositeMatrix(1.0f);
	
	// traverse scene graph from root note to build out vector of objects in scene graph
	traverseSceneGraph(root, compositeMatrix);

	std::cout << "number of objects: " << sceneObjects.size() << std::endl;
	pixelWidth = w();
	pixelHeight = h();

	updateCamera(pixelWidth, pixelHeight);
	const glm::vec3 eyePoint = getEyePoint();
	if (pixels != NULL) {
		delete pixels;
	}	
	pixels = new GLubyte[pixelWidth  * pixelHeight * 3];
	memset(pixels, 0, pixelWidth  * pixelHeight * 3);

	for (int i = 0; i < pixelWidth; i++) {
		for (int j = 0; j < pixelHeight; j++) {
            
			// storing information to find shortest intersection path
			float t = -1.0f;
			float currT = 0.0f;
			SceneObject* objToLight = nullptr;
            bool isectFound = false; 
			glm::vec3 ray = generateRay(i, j);

			// find shortest intersection path
			for (auto& it : sceneObjects) {	
				currT = renderShape(it.primitive->type, ray, it.transformMatrix, eyePoint);
				if (currT > INTERSECTION_EPSILON && (t < 0 || currT < t)) {
					t = currT;
					objToLight = &it;
				}
            }

            if (objToLight != nullptr) {
                    switch (objToLight->primitive->type) {
                        case SHAPE_CUBE:
                            shape = cube;
                            break;
                        case SHAPE_CYLINDER:
                            shape = cylinder;
                            break;
                        case SHAPE_CONE:
                            shape = cone;
                            break;
                        case SHAPE_SPHERE:
                            shape = sphere;
                            break;
                        default:
                            shape = cube;
                    }

                }
			
            if (isectOnly == 1) {
				if (t != -1 && t > 0) {
					setpixel(pixels, i, j, 255, 255, 255);
				}
			} else if (objToLight != nullptr && t > 0) {// t!=1
				glm::vec3 color = traceRay(eyePoint, ray, 0, recurseDepth);
				glm::vec3 gammaCorrection = pow(color, glm::vec3(1.0 / 2.2));
				setpixel(pixels, i, j,
					color.r * 255,
					color.g * 255,
					color.b * 255);
            }
		}
	}
	cout << "render complete" << endl;
	redraw();
}

// New helper function to find closest intersection
IntersectionInfo MyGLCanvas::findClosestIntersection(const glm::vec3& origin, const glm::vec3& ray) {
	IntersectionInfo result;
	result.t = -1;
	result.object = nullptr;

	float currT;
	for (auto& obj : sceneObjects) {
		currT = renderShape(obj.primitive->type, ray, obj.transformMatrix, origin);
		if (currT > INTERSECTION_EPSILON && (result.t < 0 || currT < result.t)) {
			result.t = currT;
			result.object = &obj;
		}
	}

	if (result.object != nullptr) {
		result.point = origin + result.t * ray;
		// Set the shape based on the object type
		switch (result.object->primitive->type) {
		case SHAPE_SPHERE: shape = sphere; break;
		case SHAPE_CUBE: shape = cube; break;
		case SHAPE_CYLINDER: shape = cylinder; break;
		case SHAPE_CONE: shape = cone; break;
		default: shape = cube;
		}
		result.normal = shape->drawNormal(origin, result.point, result.object->transformMatrix, result.t);
	}

	return result;
}

glm::vec3 MyGLCanvas::traceRay(const glm::vec3& origin, const glm::vec3& ray, int depth, int maxDepth) {
    // reached max recursion depth, exit recursion 
    if (depth > maxDepth) {
        return glm::vec3(0.0f);
    }

    IntersectionInfo isect = findClosestIntersection(origin, ray);
    if (!isect.object) {
        return glm::vec3(0.0f);  
    }

    float blend = isect.object->primitive->material.blend;
    glm::mat4 inverseMatrix = glm::inverse(isect.object->transformMatrix);

    // convert to object space
    glm::vec3 objPoint = glm::vec3(inverseMatrix * glm::vec4(isect.point, 1.0f));
    glm::vec2 uv = shape->getUVCoordinates(objPoint);

    // texture mapping 
    string textureFileName = isect.object->primitive->material.textureMap->filename;
    float texture_r = 0.0f;
    float texture_g = 0.0f;
    float texture_b = 0.0f;
    glm::vec3 textureMap = glm::vec3(texture_r, texture_g, texture_b);

    // apply texture map if given 
    if (isect.object->primitive->material.textureMap->isUsed) {
        int i = isect.object->primitive->material.textureMap->repeatU;
        int j = isect.object->primitive->material.textureMap->repeatV;
        auto it = textureCache.find(textureFileName);
        if (it == textureCache.end()) {
            textureCache[textureFileName] = new ppm(textureFileName);
            it = textureCache.find(textureFileName);
        }

        char* color = it->second->getPixels();
        float s = uv.x;
        float t = uv.y;
        int width = it->second->getWidth();
        int height = it->second->getHeight();

        s *= i;
        t *= j;
        s = fmod(s, 1.0f);
        t = fmod(t, 1.0f);

        if (s < 0) s += 1.0f;
        if (t < 0) t += 1.0f;
        int pixelS = static_cast<int>((s) * (width - 1));
        int pixelT = static_cast<int>((1.0f - t) * (height - 1));

        int pixelIndex = (pixelT * width + pixelS) * 3;
        pixelIndex = std::min(pixelIndex, (width * height * 3) - 3);
        texture_r = float((unsigned char)(color[pixelIndex])) / 255.0f;
        texture_g = float((unsigned char)(color[pixelIndex + 1])) / 255.0f;
        texture_b = float((unsigned char)(color[pixelIndex + 2])) / 255.0f;
    } 
    textureMap = glm::vec3(texture_r, texture_g, texture_b);

    float r = calculateObjectLighting(*isect.object, isect.normal, RED, parser->getNumLights(), isect.point, blend, textureMap, origin);
    float g = calculateObjectLighting(*isect.object, isect.normal, GREEN, parser->getNumLights(), isect.point, blend, textureMap, origin);
    float b = calculateObjectLighting(*isect.object, isect.normal, BLUE, parser->getNumLights(), isect.point, blend, textureMap, origin);

    glm::vec3 directColor(r, g, b);

    // calculate reflection with if material is reflective 
    float kr = globalSceneData.ks; 
    if (kr > 0.0f && depth < maxDepth) {
        glm::vec3 v = glm::normalize(ray);
        glm::vec3 reflectedRay = v - 2 * glm::dot(v, isect.normal) * isect.normal;

        // reflected color 
        glm::vec3 reflectedColor = traceRay(isect.point + SHADOW_EPSILON * isect.normal,
            reflectedRay,
            depth + 1,
            maxDepth);

        // apply material reflective properties
        reflectedColor = glm::vec3(
            reflectedColor.r * isect.object->primitive->material.cReflective.r,
            reflectedColor.g * isect.object->primitive->material.cReflective.g,
            reflectedColor.b * isect.object->primitive->material.cReflective.b
        );

        // add reflective coefficient kr
        directColor += kr * reflectedColor;
    }

    return glm::clamp(directColor, 0.0f, 1.0f);
}