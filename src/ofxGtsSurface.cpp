#include "ofxGtsSurface.h"
#include "ofMain.h"


static void removeFace(GtsFace* face, GtsSurface* removeFrom){
    cout << "removing face " << face << endl;
    return gts_surface_remove_face(removeFrom, face);
}

static ofVec3f toOf(GtsVertex* vec){
    return ofVec3f(vec->p.x,vec->p.y,vec->p.z);
}

static void toGts(GtsVertex* gtsVec, ofVec3f ofVec){
    gtsVec->p.x = ofVec.x;
    gtsVec->p.y = ofVec.y;
    gtsVec->p.z = ofVec.z;
}

static void apply_matrix (gpointer data, ofMatrix4x4* mat){
    toGts((GtsVertex*)data, toOf((GtsVertex*)data)*(*mat));
}

static void build_vertex_vector(gpointer data, vector<GtsVertex*>* vertices) {
	vertices->push_back((GtsVertex*)data);
}

static void build_edge_vector(gpointer data, vector<GtsEdge*>* edges) {
	edges->push_back((GtsEdge*)data);
}

static void build_triangle_vector(gpointer data, vector<GtsTriangle*>* triangles) {
	triangles->push_back((GtsTriangle*)data);
}

static void prepend_triangle_bbox (GtsTriangle * t, GSList ** bboxes) {
	*bboxes = g_slist_prepend (*bboxes, 
							   gts_bbox_triangle (gts_bbox_class (), t));
}


ofxGtsSurface::ofxGtsSurface() {
	surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
							  GTS_FACE_CLASS(gts_nface_class()), 
							  GTS_EDGE_CLASS(gts_nedge_class()), 
							  GTS_VERTEX_CLASS(gts_nvertex_class()));
    boolPerformed = false;
    loaded = true;
    temp = NULL;
    tree1 = tree2 = NULL; 
    si = NULL;

}

ofxGtsSurface::~ofxGtsSurface() {
    /*
	{
		vector<GtsVertex*>::iterator it = vertices.begin();
		while(it != vertices.end()) {
			delete *it;
			++it;
		}
	}
	{
		vector<GtsEdge*>::iterator it = edges.begin();
		while(it != edges.end()) {
			delete *it;
			++it;
		}
	}
	{
		vector<GtsFace*>::iterator it = faces.begin();
		while(it != faces.end()) {
			delete *it;
			++it;
		}
	}
    */
    if(loaded) {
		/* destroy surfaces and intersection */
		gts_object_destroy(GTS_OBJECT(surface));		
		
		if(boolPerformed) {
            gts_object_destroy(GTS_OBJECT(temp));
			gts_object_destroy(GTS_OBJECT(si));
			
			/* destroy bounding box trees (including bounding boxes) */
			gts_bb_tree_destroy(tree1, true);
			gts_bb_tree_destroy(tree2, true);  
		}
	}
    
}

void ofxGtsSurface::setup(GtsSurface* s){
    surface = s;
    loaded = true;
}

void ofxGtsSurface::setup(string filename) {
    
	FILE * fptr;
	GtsFile * fp;
	
	string filePath = ofToDataPath(filename);
	
	/* open first file */
	if ((fptr = fopen (filePath.c_str(), "rt")) == NULL) {		
		ofLog(OF_LOG_ERROR, "ofxGtsSurface::setup(file) -- Cannot open file: " + filePath);
		return;
	}
    
	/* reads in first surface file */
	surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
								  GTS_FACE_CLASS(gts_nface_class()), 
								  GTS_EDGE_CLASS(gts_nedge_class()), 
								  GTS_VERTEX_CLASS(gts_nvertex_class()));
	fp = gts_file_new(fptr);
	if (0 != gts_surface_read (surface, fp)) {
		ofLog(OF_LOG_ERROR, filePath + " is not a valid GTS surface file");
		loaded = false;
		gts_object_destroy(GTS_OBJECT(surface));
	} else {
		ofLog(OF_LOG_NOTICE, "Gts surface file read: " + filePath);
		loaded = true;
	}
	gts_file_destroy (fp);
	fclose (fptr);	
    loaded = true;
}

void ofxGtsSurface::setup(ofMesh& mesh){
    vector<GtsVertex*> verts;
    for(int i = 0; i < mesh.getVertices().size(); i++){
        verts.push_back(gts_vertex_new(gts_vertex_class(), mesh.getVertices()[i].x, mesh.getVertices()[i].y, mesh.getVertices()[i].z));
    }
    
    for(int i = 0; i < mesh.getIndices().size(); i+=3){
        GtsEdge* e1 = gts_edge_new(gts_edge_class(), verts[ mesh.getIndices()[i  ] ], verts[ mesh.getIndices()[i+1] ]);
        GtsEdge* e2 = gts_edge_new(gts_edge_class(), verts[ mesh.getIndices()[i  ] ], verts[ mesh.getIndices()[i+2] ]);
        GtsEdge* e3 = gts_edge_new(gts_edge_class(), verts[ mesh.getIndices()[i+1] ], verts[ mesh.getIndices()[i+2] ]);
        GtsFace* face = gts_face_new(gts_face_class(), e1, e2, e3);
        gts_surface_add_face(surface, face);
    }
    
    
    
}

ofVec3f ofxGtsSurface::centroid(){
    GtsVector v;
    gts_surface_center_of_mass(surface, v);
    return ofVec3f(v[0],v[1],v[2]);
}

void ofxGtsSurface::centerAtOrigin(){
    translate(-centroid());
}

// level:  http://mathworld.wolfram.com/GeodesicDome.html
void ofxGtsSurface::setupSphere(int level) {
	gts_surface_generate_sphere(surface, level);
}

GtsVertex* ofxGtsSurface::createVertex(float x, float y, float z) {
 	GtsVertex* v = gts_vertex_new(surface->vertex_class, x, y, z);
//	vertices.push_back(v);
	return v;
}

GtsEdge* ofxGtsSurface::createEdge(GtsVertex* v1, GtsVertex* v2) {
	GtsEdge* edge = gts_edge_new(surface->edge_class, v1, v2);
//	edges.push_back(edge);
	return edge;
}

GtsFace* ofxGtsSurface::createFace(GtsEdge* e1, GtsEdge* e2, GtsEdge* e3) {
	GtsFace* face = gts_face_new(surface->face_class, e1, e2, e3);
//	faces.push_back(face);
	return face;
}

GtsSurface*	ofxGtsSurface::getGtsSurface(){
    return surface;
}

vector<GtsVertex*> ofxGtsSurface::getFaceVertices(GtsFace* face) {

}

vector<GtsVertex*> ofxGtsSurface::getVertices() {
	vector<GtsVertex*> vertices;
	gts_surface_foreach_vertex(surface, (GtsFunc) build_vertex_vector, &vertices);
	return vertices;
}

vector<GtsEdge*> ofxGtsSurface::getEdges() {
	vector<GtsEdge*> edges;
	gts_surface_foreach_edge(surface, (GtsFunc) build_edge_vector, &edges);
	return edges;
}

vector<GtsTriangle*> ofxGtsSurface::getTriangles() {
	vector<GtsTriangle*> triangles;
	gts_surface_foreach_face(surface, (GtsFunc) build_triangle_vector, &triangles);
	return triangles;
}

void ofxGtsSurface::copyTo(ofxGtsSurface* target){
    if(target->surface != NULL){
        gts_object_destroy(GTS_OBJECT(target->surface));		
        target->surface = NULL;
    }
    
	target->surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
									 GTS_FACE_CLASS(gts_nface_class()), 
									 GTS_EDGE_CLASS(gts_nedge_class()), 
									 GTS_VERTEX_CLASS(gts_nvertex_class()));
    
    gts_surface_copy(target->surface, surface);
    target->loaded = true;
}

void ofxGtsSurface::copyTo(ofMesh& mesh){
    
    copyTo(mesh.getVertices());
    
    vector<GtsTriangle*> triangles = getTriangles();
    vector<GtsVertex*> vertices = getVertices();
    mesh.clearIndices();
    for(int i = 0; i < triangles.size(); i++){
        GtsTriangle* tri = triangles[i];
        GtsVertex* a = NULL;
		GtsVertex* b = NULL;
		GtsVertex* c = NULL;
        
        gts_triangle_vertices(tri, &a, &b, &c);
        for(int v = 0; v < mesh.getVertices().size(); v++){
            if(a == NULL && b == NULL && c == NULL){
                break;
            }
            
            if(a == vertices[v]){
                mesh.addIndex(v);
                a = NULL;
            }
            if(b == vertices[v]){
                mesh.addIndex(v);                
                b = NULL;
            }
            if(c == vertices[v]){
                mesh.addIndex(v);
                c = NULL;
            }
        }        
    }
//    cout << "copied " << mesh.getIndices().size() << " Indices" << endl;
}

void ofxGtsSurface::copyTo(vector<ofVec3f>& outverts){
    outverts.clear();
    vector<GtsVertex*> inverts = getVertices();
    for(int i = 0; i < inverts.size(); i++){
        outverts.push_back(toOf(inverts[i]));
    }
}

void ofxGtsSurface::translate(ofVec3f translation){
    applyMatrix(ofMatrix4x4::newTranslationMatrix(translation));
}

void ofxGtsSurface::scale(float scale){
    applyMatrix(ofMatrix4x4::newScaleMatrix(ofVec3f(scale,scale,scale)));    
}

void ofxGtsSurface::scale(ofVec3f scale){
    applyMatrix(ofMatrix4x4::newScaleMatrix(scale));
}

void ofxGtsSurface::rotate(float angle, ofVec3f axis){
    applyMatrix(ofMatrix4x4::newRotationMatrix(angle, axis.x, axis.y, axis.z));
}

void ofxGtsSurface::rotate(ofQuaternion rotation){
    applyMatrix(ofMatrix4x4::newRotationMatrix(rotation));
}

void ofxGtsSurface::rotate(float angle, ofVec3f axis, ofVec3f pivot){
    //TODO ?
}

void ofxGtsSurface::applyMatrix(ofMatrix4x4 transform){
	gts_surface_foreach_vertex(surface, (GtsFunc) apply_matrix, &transform);
}

bool ofxGtsSurface::prepareBoolean(ofxGtsSurface &source) {
	if(!loaded || !source.loaded) {
		ofLog(OF_LOG_NOTICE, "Gts surface not loaded, could not perform boolean operation");
		return false;
	}
	
    GtsSurface* self_inter;
	if(!gts_surface_is_orientable(surface)) {
		ofLog(OF_LOG_ERROR, "Gts surface is not an orientable manifold, could not perform boolean operation");
		return false;
	}
	
    
    self_inter = gts_surface_is_self_intersecting(surface);
	if(self_inter) {
		ofLog(OF_LOG_ERROR, "Gts surface self-intersects, could not perform boolean operation");
//        gts_surface_foreach_face(self_inter, (GtsFunc)removeFace, surface);
		return false;
	}
	
	if(!gts_surface_is_orientable(source.surface)) {
		ofLog(OF_LOG_ERROR, "Gts source surface is not an orientable manifold, could not perform boolean operation");
		return false;
	}
	
    self_inter = gts_surface_is_self_intersecting(source.surface);
	if(self_inter) {
//        gts_surface_foreach_face(self_inter, (GtsFunc)removeFace, source.surface);
		ofLog(OF_LOG_ERROR, "Gts source surface self-intersects, could not perform boolean operation");
		return false;
	}
	
    /*
    if(si != NULL){
        gts_object_destroy(GTS_OBJECT(si));
        si = NULL;
    }
    
    if(tree1 != NULL){
        gts_bb_tree_destroy(tree1, true);
        tree1 = NULL;
    }
    if(tree2 != NULL){
        gts_bb_tree_destroy(tree2, true);
        tree2 = NULL;
    }
    */
        
//    if(boolPerformed) {
//        gts_object_destroy(GTS_OBJECT(si));
//        
//        /* destroy bounding box trees (including bounding boxes) */
//        gts_bb_tree_destroy(tree1, true);
//        gts_bb_tree_destroy(tree2, true);  
//        boolPerformed = false;
//    }

	GSList *bboxes = NULL;
	gts_surface_foreach_face(surface, (GtsFunc) prepend_triangle_bbox, &bboxes);
    if(bboxes == NULL){
        ofLogError("Gts bboxes is null");
        return false;
    }
    
	/* build bounding box tree for first surface */
	tree1 = gts_bb_tree_new(bboxes);
	/* free list of bboxes */
	g_slist_free (bboxes);
	is_open1 = gts_surface_volume(surface) < 0.;
	
	/* build bounding boxes for second surface */
	bboxes = NULL;
	gts_surface_foreach_face(source.surface, (GtsFunc) prepend_triangle_bbox, &bboxes);
	/* build bounding box tree for second surface */
	tree2 = gts_bb_tree_new(bboxes);
	/* free list of bboxes */
	g_slist_free (bboxes);
	is_open2 = gts_surface_volume (source.surface) < 0.;
	si = gts_surface_inter_new (gts_surface_inter_class(), 
								surface, source.surface, tree1, tree2, is_open1, is_open2);

	boolPerformed = true;
    
	gboolean closed = TRUE;
	gts_surface_inter_check(si, &closed);
    
	if(!closed) {
		ofLog(OF_LOG_NOTICE, "Gts surface is not closed, could not perform boolean operation");
		return false;
	}    
	return true;
}

void ofxGtsSurface::createBoolean(ofxGtsSurface &source, ofxGtsSurface &result, BooleanOperation operation) {
    
    if(temp == NULL){
        temp = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
                               GTS_FACE_CLASS(gts_nface_class()), 
                               GTS_EDGE_CLASS(gts_nedge_class()), 
                               GTS_VERTEX_CLASS(gts_nvertex_class()));
    }
    
	switch(operation) {
			
		case BOOLEAN_INTERSECTION:
			gts_surface_inter_boolean(si, temp, GTS_1_IN_2);
			gts_surface_inter_boolean(si, temp, GTS_2_IN_1);
			break;
			
		case BOOLEAN_UNION:
			gts_surface_inter_boolean(si, temp, GTS_1_OUT_2);
			gts_surface_inter_boolean(si, temp, GTS_2_OUT_1);
			break;
			
		case BOOLEAN_DIFFERENCE:
			gts_surface_inter_boolean(si, temp, GTS_1_OUT_2);
			gts_surface_inter_boolean(si, temp, GTS_2_IN_1);
			gts_surface_foreach_face(si->s2, (GtsFunc)gts_triangle_revert, NULL);
			gts_surface_foreach_face(source.surface, (GtsFunc)gts_triangle_revert, NULL);
			break;
			
		case BOOLEAN_REVERSE_DIFFERENCE:
			// TODO: Reverse difference can cause crashes, is there a way to catch them?
			gts_surface_inter_boolean(si, temp, GTS_2_OUT_1);
			gts_surface_inter_boolean(si, temp, GTS_1_IN_2);
			gts_surface_foreach_face(si->s1, (GtsFunc)gts_triangle_revert, NULL);
			gts_surface_foreach_face(surface, (GtsFunc)gts_triangle_revert, NULL);
			break;
	}
        
	result.surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
									 GTS_FACE_CLASS(gts_nface_class()), 
									 GTS_EDGE_CLASS(gts_nedge_class()), 
									 GTS_VERTEX_CLASS(gts_nvertex_class()));

    gts_surface_copy(result.surface, temp);
    GtsSurface* self_inter = gts_surface_is_self_intersecting (result.surface);
    if(self_inter){
        ofLogError("ofxGtsSurface::createBoolean -- Resulting shape is self-intersecting");
//        gts_surface_foreach_face(self_inter, (GtsFunc)removeFace, result.surface);
    }

    //result.surface = temp;
    result.loaded = true;	
}

void ofxGtsSurface::getUnion(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_UNION);
	} 
}

void ofxGtsSurface::getIntersection(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_INTERSECTION);
	} 
}

void ofxGtsSurface::getDifference(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_DIFFERENCE);
	} 					  
}

void ofxGtsSurface::getReverseDifference(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_REVERSE_DIFFERENCE);
	} 					  
}