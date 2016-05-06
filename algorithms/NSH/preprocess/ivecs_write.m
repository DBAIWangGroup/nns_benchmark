% This function writes a vector from a file in the ivecs format

function ivecs_write (filename, v)

% open the file and count the number of descriptors
fid = fopen (filename, 'wb');
d = size (v, 1);
n = size (v, 2);

for i = 1:n
  
  % first write the vector size
  count = fwrite (fid, d, 'int');

  if count ~= 1 
    error ('Unable to write vector dimension: count !=1 \n');
  end

  % write the vector components
  count = fwrite (fid, v(:,i), 'int');

  if count ~= d 
    error ('Unable to write vector elements: count !=1 \n');
  end
end

fclose (fid);
