%%%% Transform database and query

function transform_base_query(data_path,query_path,Rinit_path,trans_data_path, trans_query_path)

R_opq_p = fvecs_read(Rinit_path);
dataset = fvecs_read(data_path);

dataset = R_opq_p' * dataset;
fvecs_write(trans_data_path,dataset);

query = fvecs_read(query_path);
query = R_opq_p' * query;

fvecs_write (trans_query_path,query);

end
