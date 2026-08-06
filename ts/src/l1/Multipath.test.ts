// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Multipath } from './Multipath';
import { Converter } from './Converter';

type Point = { x: number; y: number } | null;
type Rect  = { x: number; y: number; width: number; height: number } | null;

class ParsePoint implements Converter<string, Point> {
    resolve(s: string): Point {
        const parts = s.split(',').map(Number);
        return parts.length === 2 && parts.every(Number.isFinite)
            ? { x: parts[0], y: parts[1] }
            : null;
    }
}

class ExpandToRect implements Converter<Point, Rect> {
    constructor(private readonly size: number) {}
    resolve(p: Point): Rect {
        return p && { x: p.x, y: p.y, width: this.size, height: this.size };
    }
}

class ParsePointAlt implements Converter<string, Point> {
    resolve(s: string): Point {
        const parts = s.split(' ').map(Number);
        return parts.length === 2 && parts.every(Number.isFinite)
            ? { x: parts[0], y: parts[1] }
            : null;
    }
}

describe('Multipath', () => {
    it('to() chains steps and returns a Multipath', () => {
        const path: Multipath<string, Rect> = new Multipath<string, Point>()
            .to(new ParsePoint())
            .to(new ExpandToRect(100));
        assert.deepEqual(path.traverse('10,20', {}), { x: 10, y: 20, width: 100, height: 100 });
        assert.equal(path.traverse('bad', {}), null);
    });

    it('toEither() embeds a Chain inline and tries converters in order', () => {
        const path: Multipath<string, Rect> = new Multipath<string, Point>()
            .toEither(new ParsePoint(), new ParsePointAlt())
            .to(new ExpandToRect(50));
        assert.deepEqual(path.traverse('5,10', {}),   { x: 5, y: 10, width: 50, height: 50 });  // comma form
        assert.deepEqual(path.traverse('5 10', {}),   { x: 5, y: 10, width: 50, height: 50 });  // space form
        assert.equal(path.traverse('bad', {}), null);
    });

    it('is assignable to Converter', () => {
        const path: Converter<string, Rect> = new Multipath<string, Point>()
            .to(new ParsePoint())
            .to(new ExpandToRect(10));
        assert.deepEqual(path.resolve('1,2', {}), { x: 1, y: 2, width: 10, height: 10 });
    });
});
