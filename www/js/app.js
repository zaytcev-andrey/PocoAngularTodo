// js/app.js
var pocoTodo = angular.module('pocoTodo', []);

function CostBasisMin( value ) {    
    var value_ = 
    ( value === 'undefined' || isNaN(value) ) ? 0: value;
    
    this.getValue = function() {
        return value_;
    }
}

function CostBasisMax( value ) {
    var value_ = 
    ( value === 'undefined' || isNaN(value) ) ? Math.pow(2, 30): value;    
    
    this.getValue = function() {
        return value_;
    }
}

function mainController($scope, $http) {
    $scope.formData = {};
    $http.defaults.headers.post["Content-Type"] = "application/x-www-form-urlencoded";

    var config = { headers: {
            'If-Modified-Since': '2014-11-27 T14:37:00'
        }
    };
    // when landing on the page, get all doors and show them
    $http.get('/api/doors', config)
        .success(function(data) {
            $scope.doors = data;
            console.log(data);
        })
        .error(function(data) {
            console.log('Error: ' + data);
        });
		
	$scope.getSuitableDoors = function() {     
        var costMin = new CostBasisMin($scope.formData.costbasismin);
        var costMax = new CostBasisMax($scope.formData.costbasismax);
        
        var get_doors_by_cost = '/api/doors' + 
        '/costbasismim/' + costMin.getValue() +
        '/costbasismax/' + costMax.getValue();

        $http.get(get_doors_by_cost)
            .success(function(data) {
                $scope.doors = data;
                console.log(data);
            })
            .error(function(data) {
                console.log('Error: ' + data);
            });
	}

    // when submitting the add form, send the text to the node API
    $scope.createTodo = function() {
        $http.post('/api/todos', $.param($scope.formData)) // $scope.formData)
            .success(function(data) {
                $scope.formData = {}; // clear the form so our user is ready to enter another
                $scope.todos = data;
                console.log(data);
            })
            .error(function(data) {
                console.log('Error: ' + data);
            });
    };

    // delete a todo after checking it
    $scope.deleteTodo = function(id) {
        $http.delete('/api/todos/' + id)
            .success(function(data) {
                $scope.todos = data;
                console.log(data);
            })
            .error(function(data) {
                console.log('Error: ' + data);
            });
    };

}

