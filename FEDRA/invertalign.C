/*inverting affine transformations (A.Iuliano 30 June 2021)
nota bene: fromplate is defined as the starting plate in the current (available) transformation.
For example, if you aligned from the last to the first, it will be the larger number (from plate =21, to plate=20)
It will be created (or updated) an AFF file from 20 to 21*/

void invertalign(int fromplate, int toplate, int setnumber = 1, int major = 0, int minor = 0){
 EdbScanProc *sproc = new EdbScanProc();
 sproc->eProcDirClient = "..";
 //currently available transformation
 int fromid[4] = {setnumber,fromplate,major,minor};
 int toid[4] = {setnumber,toplate,major,minor};   
 EdbAffine2D affine, affineTXTY;
 float dz;
 //reading file with current aff file
 sproc->GetAffZ(affine,dz,fromid,toid);
 //getting also TXTY affine transformation
 EdbLayer layer;
 sproc->ReadAffToLayer(layer, fromid,toid); 
 affineTXTY = layer.GetAffineTXTY();
 //inverting the affine transformation, now affine contains the inverted one
 affine.Print();
 affine.Invert();
 affine.Print();
 affineTXTY.Print();
 affineTXTY.Invert();
 affineTXTY.Print();
 //building layer with inverted transformation;
 EdbLayer l;
 l.SetZcorr(-1. * dz); //inverting dz
 l.SetAffXY(affine.A11(),affine.A12(),affine.A21(),affine.A22(),affine.B1(),affine.B2());
 l.SetAffTXTY(affineTXTY.A11(),affineTXTY.A12(),affineTXTY.A21(),affineTXTY.A22(),affineTXTY.B1(),affineTXTY.B2()); 
//saving the output
 TString outputfilename;
 sproc->MakeAffName(outputfilename,toid,fromid,"aff.par");
 sproc->UpdateAFFPar(toid,fromid,l);
 cout<<outputfilename.Data()<<endl;
}
