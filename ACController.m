classdef ACController < handle
    properties (Access = private)
        conn
    end
    methods
        function obj = ACController(port)
            obj.conn = serial(port);
            fopen(obj.conn);
        end
        function delete(obj)
            fclose(obj.conn);
            delete(obj.conn);
        end
        function status(obj)
            obj.exec('STATUS');
        end
        function run(obj, intervals)
            obj.exec(['RUN ', num2str(intervals, '%u, ')]);
        end
        function stop(obj)
            obj.exec('STOP');
        end
    end
    methods (Access = private)
        function exec(obj, cmd)
            fprintf(obj.conn, cmd);
            output = fscanf(obj.conn);
            disp(deblank(output));
        end
    end
end
