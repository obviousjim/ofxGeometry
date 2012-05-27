#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(33,33,33);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
    /*
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
//	 guint nv = 1;
//	  gts_surface_foreach_vertex (s, (GtsFunc) write_vertex, &nv);
//	  cout << vertices.size() << " num vertices" << endl;
	}		
	*/
    

    sphere = gts.createSphere();

}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){

	glPushMatrix();
    glTranslatef(ofGetWidth()/2,ofGetHeight()/2,-50);
    glRotatef(sin(ofGetElapsedTimef()*1.3)*180,0,1,0);
    glScalef(100,100,100);
    ofSetColor(255);

        
    ofMesh m;
	sphere->copyToMesh(m);
    m.drawWireframe();
    
    glEnd();
	glPopMatrix();


}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 's'){
        ofMesh m;
        sphere->copyToMesh(m);
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