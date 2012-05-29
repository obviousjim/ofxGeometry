#pragma once
#include <gts.h>
#include <vector>
#include "ofMain.h"


class ofxGtsSurface {
  public: 
	ofxGtsSurface();
	~ofxGtsSurface();
    
    void setup(GtsSurface* surface);
    void setup(string filename);
    void setup(ofMesh& mesh);
    void setupSphere(int level);
    
    void getUnion(ofxGtsSurface &source, ofxGtsSurface &result);
	void getIntersection(ofxGtsSurface &source, ofxGtsSurface &result);
	void getDifference(ofxGtsSurface &source, ofxGtsSurface &result);
	void getReverseDifference(ofxGtsSurface &source, ofxGtsSurface &result);

    ofVec3f centroid();
    void centerAtOrigin();
    
    void translate(ofVec3f translation);
    void scale(float scale);
    void scale(ofVec3f scale);
    void rotate(float angle, ofVec3f axis);
    void rotate(ofQuaternion rotation);
    void rotate(float angle, ofVec3f axis, ofVec3f pivot);    
    void applyMatrix(ofMatrix4x4 transform);
    
    void copyTo(vector<ofVec3f>& verts);    
    void copyTo(ofMesh& mesh);
    void copyTo(ofxGtsSurface* surface);
    
	GtsVertex* 		createVertex(float x, float y, float z);
	GtsEdge* 		createEdge(GtsVertex* v1, GtsVertex* v2);
	GtsFace* 		createFace(GtsEdge* e1, GtsEdge* e2, GtsEdge* e3);
		
	GtsSurface*				getGtsSurface();
	vector<GtsVertex*>		getFaceVertices(GtsFace* face);
	vector<GtsVertex*>		getVertices();
	vector<GtsEdge*>        getEdges();
	vector<GtsTriangle*> 	getTriangles();

  protected:
    bool loaded;
    GtsSurface* 	surface;
    GtsSurface*     temp; //used for booleans
    /* The type of boolean operation */
	enum BooleanOperation {
		BOOLEAN_INTERSECTION, 
		BOOLEAN_UNION, 
		BOOLEAN_DIFFERENCE, 
		BOOLEAN_REVERSE_DIFFERENCE
	};
	
	/* Prepare for a boolean operation, checking the validity of the surface */
	bool prepareBoolean(ofxGtsSurface &source);
	/* Create the boolean surface */
	void createBoolean(ofxGtsSurface &source, ofxGtsSurface &result, BooleanOperation operation);

	GNode *tree1, *tree2;
    GtsSurfaceInter * si;
    gboolean is_open1, is_open2;
    bool boolPerformed;

};

