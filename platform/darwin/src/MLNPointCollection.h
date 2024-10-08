#import <CoreLocation/CoreLocation.h>
#import <Foundation/Foundation.h>

#import "MLNFoundation.h"
#import "MLNOverlay.h"
#import "MLNShape.h"

/**
 An ``MLNPointCollection`` object represents a shape consisting of one or more
 disconnected vertices, specified as `CLLocationCoordinate2D` instances. The
 points in the collection may be related but are not connected spatially. For
 example, you could use a point collection to represent all the trees in an
 orchard.

 You can add point collections to the map by adding them to an ``MLNShapeSource``
 object. Configure the appearance of an ``MLNShapeSource``’s or
 ``MLNVectorTileSource``’s point collections collectively using an
 ``MLNCircleStyleLayer`` or ``MLNCircleStyleLayer`` object. To access a point
 collection’s attributes, use an ``MLNPointCollectionFeature`` object.

 You cannot add an ``MLNPointCollection`` object directly to a map view as an
 annotation. However, you can create individual ``MLNPointAnnotation`` objects
 from the `coordinates` array and add those annotation objects to the map view
 using the ``MLNMapView/addAnnotations:`` method.

 A point collection is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.3">MultiPoint</a>
 geometry in GeoJSON. Do not confuse ``MLNPointCollection`` with ``MLNPointCollection``,
 the abstract superclass of ``MLNPolyline`` and ``MLNPolyline``.
 */
MLN_EXPORT
@interface MLNPointCollection : MLNShape <MLNOverlay>

/**
 Creates and returns a ``MLNPointCollection`` object from the specified set of
 coordinates.

 @param coords The array of coordinates defining the shape. The data in this
 array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new point collection object.
 */
+ (instancetype)pointCollectionWithCoordinates:(const CLLocationCoordinate2D *)coords
                                         count:(NSUInteger)count;

/** The array of coordinates associated with the shape. */
@property (nonatomic, readonly) CLLocationCoordinate2D *coordinates NS_RETURNS_INNER_POINTER;

/** The number of coordinates associated with the shape. */
@property (nonatomic, readonly) NSUInteger pointCount;

/**
 Retrieves one or more coordinates associated with the shape.

 @param coords On input, you must provide a C array of structures large enough
 to hold the desired number of coordinates. On output, this structure
 contains the requested coordinate data.
 @param range The range of points you want. The `location` field indicates the
 first point you are requesting, with `0` being the first point, `1` being
 the second point, and so on. The `length` field indicates the number of
 points you want. The array in _`coords`_ must be large enough to accommodate
 the number of requested coordinates.
 */
- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range;

@end
