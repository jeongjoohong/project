function w = test_convolution(u, v, option)
    w = zeros(1, length(u) + length(v) - 1);
    for i = 1:length(u)+length(v)-1
        index = i;
        tmp = 0;
        for j = 1:length(v)
            if (index >= 1) && ~(index > length(u))
                tmp = tmp + u(index) * v(j);
            end
            index = index - 1;
            w(i) = tmp;
        end
    end
    if strcmp(option, 'same')
        start = ceil((length(w) - length(u)) / 2);
        w = w(start+1:start+length(u));
    end
end