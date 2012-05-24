#include "testApp.h"

static void write_vertex(GtsVertex* v, guint* nv) {
	testApp* app = static_cast<testApp*>(ofGetAppPtr());
	ofVec3f p(
		GTS_POINT(v)->x
		,GTS_POINT(v)->y
		,GTS_POINT(v)->z
	);
	app->vertices.push_back(p);
}

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(33,33,33);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	string file = ofToDataPath("bunny.gts",true);
	GtsSurface* s;
	GtsFile* fp;
	s = gts_surface_new(
			gts_surface_class()
			,gts_face_class()
			,gts_edge_class()
			,gts_vertex_class()
	);
	FILE* cfile;
	cfile = fopen(file.c_str(), "r");
	fp = gts_file_new(cfile);
	if(gts_surface_read(s,fp)) {
		cout << "file is not a valid gts file." << endl;
	}
	else {
	 guint nv = 1;
	  gts_surface_foreach_vertex (s, (GtsFunc) write_vertex, &nv);
	  cout << vertices.size() << " num vertices" << endl;
	}		
	
	ofxGts gts;
	ofxGtsSurface* surf = gts.createSurface();
	
	surf->createSphere(4);
	test_verts = surf->getVertices();
	vector<GtsVertex*>::iterator it = test_verts.begin();
	edges = surf->getEdges();
    triangles = surf->getTriangles();
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	glColor3f(1,1,1);

	m.clearIndices();
    m.clearVertices();
    
    vector<GtsVertex*>::iterator it = test_verts.begin();
    for(int i = 0; i < test_verts.size(); i++){
        m.getVertices().push_back(ofVec3f(test_verts[i]->p.x,test_verts[i]->p.y,test_verts[i]->p.z)); 
    }
    
    for(int i = 0; i < triangles.size(); i++){
        GtsTriangle* tri = triangles[i];
        GtsVertex* a = NULL;
		GtsVertex* b = NULL;
		GtsVertex* c = NULL;
        
        gts_triangle_vertices(tri, &a, &b, &c);
        for(int v = 0; v < test_verts.size(); v++){
            if(a == NULL && b == NULL && c == NULL){
                break;
            }
            
            if(a == test_verts[v]){
                m.addIndex(v);
                a = NULL;
            }
            if(b == test_verts[v]){
                m.addIndex(v);                
                b = NULL;
            }
            if(c == test_verts[v]){
                m.addIndex(v);
                c = NULL;
            }
        }        
//        vd.addVertex(a->p.x, a->p.y, a->p.z);
//        vd.addVertex(b->p.x, b->p.y, b->p.z);
//        vd.addVertex(c->p.x, c->p.y, c->p.z);
    }

    
	glPushMatrix();
    glTranslatef(0,0,-100);
    glRotatef(sin(ofGetElapsedTimef()*1.3)*180,0,1,0);
    glScalef(22,22,22);
    ofSetColor(255);
    m.drawWireframe();
    //m.drawVertices();
    glEnd();
	glPopMatrix();


//    while(it != test_verts.end()) {
//        m.addVertex((*it)->p.x,it->p.y,it->p.z);
//        glVertex3dv(&(*it)->p.x);
//        ++it;
//    }


    /*
	// show vertices of sphere
	glPushMatrix();
		glColor3f(1, 0, 0.5);
		glTranslatef(0,0,-100);
		glRotatef(sin(ofGetElapsedTimef()*1.3)*180,0,1,0);
		glScalef(22,22,22);
		glBegin(GL_POINTS); {
			vector<GtsVertex*>::iterator it = test_verts.begin();
			while(it != test_verts.end()) {
				glVertex3dv(&(*it)->p.x);
				++it;
			}
		}
		glEnd();
	glPopMatrix();
	
     */
    
    /*
	
	// show edges
	glPushMatrix();
		glColor3f(0, 1, 0.5);
		glTranslatef(0,0,-100);
		glRotatef(sin(ofGetElapsedTimef()*1.3)*180,0,1,0);
		float s = 15 + sin(ofGetElapsedTimef()*1.3) * 5;
		glScalef(s,s,s);
		glBegin(GL_LINES);
		{
			vector<GtsEdge*>::iterator it = edges.begin();
			while(it != edges.end()) {
				GtsEdge& edge = *(*it);
				glVertex3dv(&edge.segment.v1->p.x);
				glVertex3dv(&edge.segment.v2->p.x);
				++it;
			}
	
		}
		glEnd();
	glPopMatrix();
	
     */
    
	// show vertices of bunny
/*
	glPushMatrix();
		glColor3f(1,1,1);
		glTranslatef(-40,10,-100);	
		glScalef(200,200,200);
		glBegin(GL_POINTS);
		{
			
			vector<ofVec3f>::iterator it = vertices.begin();
			while(it != vertices.end()) {
				glVertex3fv((*it).getPtr());
				++it;
			}
		}
		glEnd();
	glPopMatrix();
	*/
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 's'){
        m.save("meshTest.ply");
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}