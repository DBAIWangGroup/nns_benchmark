function main_c(data_path,data_name,s)
    get_anchor(data_path,data_name);
    main_c_OneLayer(data_path,data_name,s);
    main_c_TwoLayer(data_path,data_name,s);
end