import numpy
with open('gEC.dat', 'r') as f:
    with open('gEC_new.dat', 'w') as f_new:
        data = f.readlines()
    
        for line in data:
            array = line.split()
            array_np = numpy.asarray(array)
            highValY = max(array_np)
            low_values_indices = array_np < highValY
            array_np[low_values_indices] = 0  
            # array_processed = numpy.insert(array_processed, 0, values=array_np, axis=0)
            f_new.write(str(array_np)+' \n')
            # numpy.savetxt(f_new, array_np)

# sed -i "s/'//g" gEC_new.dat
# sed -i "s/\[//" gEC_new.dat
# sed -i "s/\]//" gEC_new.dat
