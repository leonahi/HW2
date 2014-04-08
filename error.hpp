inline void checkErr(cl_int err, string name)
{
  if(err != CL_SUCCESS)
  {
    cerr<<"ERROR: "<<name<<" ("<<err<<") "<<endl;
    exit(1);
  }
}
 
