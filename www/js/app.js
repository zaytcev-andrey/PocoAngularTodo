// js/app.js
var pocoTodo = angular.module('pocoTodo', ['ui-rangeSlider', 'ui.multopleselector'])

pocoTodo.directive('tdMydir', ['$timeout', function ($timeout) {
    return {
        link: function ($scope, element, attrs) {
			console.log('under directive');
            $scope.$on('doorsloaded', function () {
                $timeout(function () { // You might need this timeout to be sure its run after DOM render.
					//angular.element('.scrolly').scrolly();
					
					console.log('under doorsloaded');
					
					var $nav_a = $('#nav a');
					
					$nav_a
					.scrolly()
					.on('click', function(e) {

						console.log('i am on click!!!!!!!');
						
						var t = $(this),
							href = t.attr('href');

						if (href[0] != '#')
							return;

						e.preventDefault();

						// Clear active and lock scrollzer until scrolling has stopped
							$nav_a
								.removeClass('active')
								.addClass('scrollzer-locked');

						// Set this link to active
							t.addClass('active');

					});


					// Initialize scrollzer.
					var ids = [];

					$nav_a.each(function() {

						var href = $(this).attr('href');

						if (href[0] != '#')
							return;

						ids.push(href.substring(1));

						console.log(href.substring(1));
					});
					
					console.log(ids);
					
					console.log($.scrollzer);

					angular.element.scrollzer(ids, { pad: 200, lastHack: true }); // it works, is equivalent following string
					//$.scrollzer(ids, { pad: 200, lastHack: true });
					
                }, 0, false);
            })
        }
    };
}]);

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

function SortByKey(array, key) {
    return array.sort(function(a, b) {
        var x = a[key]; var y = b[key];
        return ((x < y) ? -1 : ((x > y) ? 1 : 0));
 });
}

function mainController($scope, $http) {
    $scope.formData = {
        constructionprotection: 2,
        protectionclassesmin: 0,
        protectionclassesmax: 0,
        protectionclasses_ids: [],
        lockscount: 1
    };
	$scope.protectionclasses = { classes: {}, min: 0, max: 300 };
    $http.defaults.headers.post["Content-Type"] = "application/x-www-form-urlencoded";

    var config = { headers: {
            'If-Modified-Since': '2014-11-27 T14:37:00'
        }
    };
		
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
				$scope.$broadcast('doorsloaded');
            })
            .error(function(data) {
                console.log('Error: ' + data);
            });
	}
	
	$scope.getDoorsManufacturers = function() {
		$http.get('/api/doors/manufacturers', config)
        .success(function(data) {
            $scope.manufacturers = data;
            console.log(data);
        })
        .error(function(data) {
            console.log('Error: ' + data);
        });
	}
	
	$scope.getDoorsProtectionClasses = function() {
		$http.get('/api/doors/protectionclasses', config)
        .success(function(data) {
			console.log('Protection Classes !!!!!');
			SortByKey(data, 'protectclassname');
			$scope.protectionclasses = { classes: data, min: data[0]._id, max: data[data.length-1]._id };            
			console.log($scope.protectionclasses);
			console.log('End Protection Classes !!!!!');
			$scope.formData.protectionclasses_ids = [
            $scope.protectionclasses.classes[0]._id,
            $scope.protectionclasses.classes[1]._id].join(',');
        })
        .error(function(data) {
			console.log('Error Protection Classes !!!!!');
            console.log('Error: ' + data);
        });
	}

	$scope.getDoorsLocksCount = function () {
	    $scope.lockscounts = [
            { _id: 1, name: '1' },
	        { _id: 2, name: '2' }];
	}
	
	$scope.getDoorsManufacturers();
	$scope.getDoorsProtectionClasses();
	$scope.getDoorsLocksCount();
	
	// when landing on the page, get all doors and show them
    $http.get('/api/doors', config)
        .success(function(data) {
            $scope.doors = data;
            console.log(data);
			$scope.$broadcast('doorsloaded');
        })
        .error(function(data) {
            console.log('Error: ' + data);
        });	
	

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

