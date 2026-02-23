void analytical_function(){

    TF1 *f1 = new TF1("f1","(1-(1-x)**3)**56",0,1);
    f1->Draw();

    double check_probability = f1->Eval(0.89);
    
    cout<<"Tracking probability check "<<check_probability<<endl;
}