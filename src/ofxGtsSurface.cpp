#include "ofxGtsSurface.h"
#include "ofMain.h"


static void build_vertex_vector(gpointer data, vector<GtsVertex*>* vertices) {
	vertices->push_back((GtsVertex*)data);
}

static void build_edge_vector(gpointer data, vector<GtsEdge*>* edges) {
	edges->push_back((GtsEdge*)data);
}

static void build_triangle_vector(gpointer data, vector<GtsTriangle*>* triangles) {
	triangles->push_back((GtsTriangle*)data);
}


ofxGtsSurface::ofxGtsSurface() {
	surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
							  GTS_FACE_CLASS(gts_nface_class()), 
							  GTS_EDGE_CLASS(gts_nedge_class()), 
							  GTS_VERTEX_CLASS(gts_nvertex_class()));
    
    cout << "gts constructor" << endl;
    
//    loaded = false;
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
}

void ofxGtsSurface::setup(GtsSurface* s){
    surface = s;
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
}

void ofxGtsSurface::setup(ofMesh& mesh){
    //TODO::
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

void ofxGtsSurface::copyToMesh(ofMesh& mesh){
    
    copyVertices(mesh.getVertices());
//    cout << "copied " << mesh.getVertices().size() << " verts" << endl;
    
    vector<GtsTriangle*> triangles = getTriangles();
    vector<GtsVertex*> vertices = getVertices();
//    cout << " we have " << vertices.size() << " gts verts  and " << triangles.size() << " gts triangles " << endl;
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

void ofxGtsSurface::copyVertices(vector<ofVec3f>& outverts){
    outverts.clear();
    vector<GtsVertex*> inverts = getVertices();
    for(int i = 0; i < inverts.size(); i++){
        outverts.push_back(ofVec3f(inverts[i]->p.x,
                                   inverts[i]->p.y,
                                   inverts[i]->p.z)); 
    }
    

}
